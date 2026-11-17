#include "default_data_store.hh"
#include "r_kister_data_store.hh"
#include "thermophysical_properties.hh"
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
#include <cmath>
#include <sstream>

namespace {
using nlohmann::json;
json property(std::vector<double> values) {
  return {{"values", values}, {"range", {500, 1000}},
          {"pct_uncertainty", 1}, {"uncertainty_notes", "None"},
          {"reference", "test"}};
}
json dataset() {
  json record = {{"molecular_weight", 50},
                 {"heat_capacity", property({1, 0.1, 0, 0.001})},
                 {"density", property({3, 0.001})}};
  for (const auto &name : {"melt", "boil"})
    record[name] = {{"value", std::string(name) == "melt" ? 500 : 1000},
                    {"abs_uncertainty", 0}, {"reference", "test"},
                    {"value_notes", "None"}, {"uncertainty_notes", "None"}};
  return {{"MSTDBTP", {{"evaluated", {{"A-B", {{"0.25-0.75", record}}}}}}}};
}
template<class Store> void load(Store &store, const json &data) {
  std::istringstream in(data.dump());
  store.from_json(in);
}
double enthalpy(double t) {
  return (t - 500) + 0.05 * (t * t - 500 * 500) +
         0.001 / 3 * (t * t * t - 500.0 * 500 * 500);
}
} // namespace

TEST(correctness, json_failures_are_atomic) {
  saline::Default_Data_Store store;
  load(store, dataset());
  auto view = store.setView({"A", "B"}, {0.25, 0.75});
  for (const auto &name : {"density", "viscosity", "heat_capacity",
                          "thermal_conductivity", "surface_tension"}) {
    for (const auto &values : {json::array(), json::array({1}),
                              json::array({1, 2, 3, 4, 5}), json::array({"x", 1})}) {
      auto bad = dataset();
      bad["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"][name] = property({1, 2});
      bad["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"][name]["values"] = values;
      EXPECT_THROW(load(store, bad), std::runtime_error) << name;
      EXPECT_EQ(1u, store.size());
      EXPECT_DOUBLE_EQ(2.1, view.rho(900));
    }
  }
  for (const auto &text : {"{", "{}", "[]", "null", "{\"MSTDBTP\":{\"evaluated\":[]}}"}) {
    std::istringstream bad(text);
    EXPECT_THROW(store.from_json(bad), std::runtime_error);
    EXPECT_DOUBLE_EQ(2.1, view.rho(900));
  }
  auto late_failure = dataset();
  late_failure["MSTDBTP"]["evaluated"]["Z"]["1"] = {{"molecular_weight", -1}};
  EXPECT_THROW(load(store, late_failure), std::runtime_error);
  EXPECT_EQ(1u, store.size());
  load(store, dataset());
  EXPECT_EQ(1u, store.size());
}

TEST(correctness, enthalpy_intervals_and_prerequisites) {
  saline::Default_Data_Store store;
  load(store, dataset());
  auto v = store.setView({"A", "B"}, {0.25, 0.75});
  for (double t : {500.0, 505.0, 990.0, 995.0}) {
    EXPECT_NEAR(enthalpy(t), v.h_t(t), 1e-7);
    EXPECT_NEAR(t, v.t_h(enthalpy(t)), 1e-9);
  }
  EXPECT_NEAR(977.5, v.t_h((enthalpy(975) + enthalpy(980)) / 2), 1e-9);
  EXPECT_NEAR(497.5, v.t_h(-enthalpy(505) / 2), 1e-9);
  EXPECT_NEAR(997.5, v.t_h(enthalpy(995) + (enthalpy(995) - enthalpy(990)) / 2), 1e-9);
  EXPECT_THROW(v.t_h(std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
  EXPECT_THROW(v.h_t(0), std::invalid_argument);
  auto data = dataset();
  data["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"].erase("boil");
  load(store, data);
  EXPECT_NEAR(enthalpy(900), v.h_t(900), 1e-7);
  EXPECT_THROW(v.t_h(1), std::runtime_error);
  data["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"].erase("melt");
  load(store, data);
  EXPECT_THROW(v.h_t(900), std::runtime_error);
  data = dataset();
  data["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"]["heat_capacity"] = property({-1, 0, 0, 0});
  load(store, data);
  EXPECT_THROW(v.t_h(1), std::runtime_error);
}

TEST(correctness, selection_guards_and_ordering) {
  saline::Thermophysical_Properties tp;
  EXPECT_THROW(tp.rho(900), std::runtime_error);
  EXPECT_THROW(tp.rho_rng(), std::runtime_error);
  EXPECT_THROW(tp.composition(), std::runtime_error);
  EXPECT_THROW(tp.setComposition({"A"}, {1}), std::runtime_error);
  saline::Default_Data_Store store;
  load(store, dataset());
  ASSERT_TRUE(tp.initialize(&store));
  ASSERT_TRUE(tp.setComposition({"B", "A"}, {0.7, 0.3}));
  EXPECT_EQ((std::vector<std::string>{"B", "A"}), tp.species());
  EXPECT_EQ((std::vector<double>{0.75, 0.25}), tp.composition());
  EXPECT_FALSE(tp.setComposition({"missing"}, {1}));
  EXPECT_DOUBLE_EQ(2.1, tp.rho(900));
  EXPECT_THROW(tp.setComposition({"A", "A"}, {0.5, 0.5}), std::invalid_argument);
  EXPECT_THROW(tp.setComposition({"A"}, {}), std::invalid_argument);
  EXPECT_THROW(tp.setComposition("A", nullptr, 1), std::invalid_argument);
  auto v = store.view(0);
  EXPECT_TRUE(v.nullView());
  EXPECT_THROW(v.cp(900), std::runtime_error);
  v.rec_id = 100;
  EXPECT_THROW(v.rho(900), std::runtime_error);
  ASSERT_TRUE(tp.initialize(&store));
  EXPECT_THROW(tp.rho(900), std::runtime_error);
}

TEST(correctness, rk_unsupported_operations_throw) {
  saline::R_Kister_Data_Store store;
  EXPECT_THROW(store.cp_h(0, 0, 1), std::runtime_error);
  EXPECT_THROW(store.mu_h(0, 0, 1), std::runtime_error);
  EXPECT_THROW(store.k_h(0, 0, 1), std::runtime_error);
  EXPECT_THROW(store.rho_h(0, 0, 1), std::runtime_error);
  EXPECT_THROW(store.surfaceTension(0, 0, 900), std::runtime_error);
  EXPECT_THROW(store.surfaceTension_h(0, 0, 1), std::runtime_error);
  EXPECT_THROW(store.speedOfSound_h(0, 0, 1), std::runtime_error);
  EXPECT_THROW(store.h_t(0, 0, 900), std::runtime_error);
  EXPECT_THROW(store.t_h(0, 0, 1), std::runtime_error);
  EXPECT_FALSE(store.valid_surfaceTension(0, 0));
}

TEST(correctness, rk_json_failures_are_atomic) {
  auto data = dataset();
  auto record = data["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"];
  data["MSTDBTP"]["evaluated"]["A"]["1"] = record;
  data["MSTDBTP"]["evaluated"]["B"]["1"] = record;
  data["MSTDBTP"]["estimates"]["RK"]["Density"]["A-B"] =
      {{"A", {0.0}}, {"B", {0.0}}};
  saline::R_Kister_Data_Store store;
  load(store, data);
  saline::Thermophysical_Properties tp;
  ASSERT_TRUE(tp.initialize(&store));
  ASSERT_TRUE(tp.setComposition({"A", "B"}, {0.25, 0.75}));
  double before = tp.rho(900);
  for (const auto &key : {"A", "A-missing", "A-B"}) {
    auto bad = data;
    bad["MSTDBTP"]["estimates"]["RK"]["Density"][key] =
        {{"A", {1.0, 2.0}}, {"B", {0.0}}};
    EXPECT_THROW(load(store, bad), std::runtime_error);
    EXPECT_DOUBLE_EQ(before, tp.rho(900));
  }
  EXPECT_FALSE(tp.setComposition({"missing"}, {1}));
  EXPECT_DOUBLE_EQ(before, tp.rho(900));
}

TEST(correctness, qualifiers_survive_json_round_trip) {
  for (const auto &note : {"None", "Pressurized", "Sublimates", "LiquidGas",
                           "Uncharacterized", "Modeled", "Evaluated", "Reported",
                           "Uncertanity needs to be further evaluated"}) {
    SCOPED_TRACE(note);
    auto data = dataset();
    auto &record = data["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"];
    for (const auto &name : {"viscosity", "thermal_conductivity", "surface_tension"})
      record[name] = property({1, 0.001});
    for (const auto &name : {"melt", "boil"})
      record[name]["value_notes"] = note;
    for (const auto &name : {"melt", "boil", "density", "viscosity",
                             "thermal_conductivity", "heat_capacity", "surface_tension"})
      record[name]["uncertainty_notes"] = note;

    saline::Default_Data_Store store;
    load(store, data);
    auto exported = json::parse(store.to_json());
    const auto &result = exported["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"];
    for (const auto &name : {"melt", "boil"})
      EXPECT_EQ(note, result.at(name).at("value_notes").get<std::string>());
    for (const auto &name : {"melt", "boil", "density", "viscosity",
                             "thermal_conductivity", "heat_capacity", "surface_tension"})
      EXPECT_EQ(note, result.at(name).at("uncertainty_notes").get<std::string>());
    EXPECT_EQ(result, json::parse(store.to_json_by_id(0, 0)));
    auto view = store.setView({"A", "B"}, {0.25, 0.75});
    EXPECT_DOUBLE_EQ(2.1, view.rho(900));
    EXPECT_NEAR(enthalpy(900), view.h_t(900), 1e-7);
    load(store, exported);
    EXPECT_EQ(exported, json::parse(store.to_json()));

    saline::R_Kister_Data_Store rk;
    load(rk, data);
    EXPECT_EQ(exported["MSTDBTP"]["evaluated"],
              json::parse(rk.to_json())["MSTDBTP"]["evaluated"]);
  }
}

TEST(correctness, pending_uncertainty_accepts_corrected_spelling) {
  auto data = dataset();
  data["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"]["melt"]["uncertainty_notes"] =
      "Uncertainty needs to be further evaluated";
  saline::Default_Data_Store store;
  load(store, data);
  const auto result = json::parse(store.to_json_by_id(0, 0));
  // Continue writing the established external spelling, not a new format.
  EXPECT_EQ("Uncertanity needs to be further evaluated",
            result["melt"]["uncertainty_notes"].get<std::string>());
}

TEST(correctness, speed_of_sound_metadata_round_trip) {
  const json minimal = {{"value", 2500}};
  const json annotated = {{"value", 2500}, {"reference", "Sound reference"},
                          {"DOI", "https://example.org/sound"},
                          {"value_notes", "At the melting point"},
                          {"uncertainty_notes", "Uncertainty not reported"}};
  auto ranged = annotated;
  ranged["pct_uncertainty"] = 2.5;
  ranged["range"] = {500, 1000};
  auto zero_uncertainty = annotated;
  zero_uncertainty["pct_uncertainty"] = 0;
  zero_uncertainty["range"] = {0, 0};
  for (const auto &sound : {minimal, annotated, ranged, zero_uncertainty}) {
    auto data = dataset();
    data["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"]["speed_of_sound"] = sound;
    saline::Default_Data_Store store;
    load(store, data);
    auto view = store.setView({"A", "B"}, {0.25, 0.75});
    ASSERT_TRUE(view.valid_speedOfSound());
    EXPECT_DOUBLE_EQ(2500, view.speedOfSound(500));
    EXPECT_DOUBLE_EQ(2500, view.speedOfSound(900));
    EXPECT_DOUBLE_EQ(2500, view.speedOfSound_h(view.h_t(900)));
    EXPECT_EQ(sound.value("reference", ""), view.speedOfSound_ref());
    EXPECT_DOUBLE_EQ(sound.value("pct_uncertainty", 0.0) / 100.0, view.speedOfSound_unc());
    EXPECT_EQ((sound.contains("range") ? sound["range"].get<std::pair<double, double>>()
                                       : std::make_pair(0.0, 0.0)), view.speedOfSound_rng());
    EXPECT_EQ(sound, json::parse(store.to_json_by_id(0, 0)).at("speed_of_sound"));
    auto exported = json::parse(store.to_json());
    EXPECT_EQ(sound, exported["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"]["speed_of_sound"]);
    load(store, exported);
    EXPECT_EQ(exported, json::parse(store.to_json()));
    saline::R_Kister_Data_Store rk;
    load(rk, data);
    EXPECT_EQ(sound, json::parse(rk.to_json())["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"]["speed_of_sound"]);
  }
}

TEST(correctness, invalid_speed_of_sound_is_rejected_atomically) {
  auto data = dataset();
  auto &sound = data["MSTDBTP"]["evaluated"]["A-B"]["0.25-0.75"]["speed_of_sound"];
  const json valid = {{"value", 2500}, {"reference", "unchanged"}};
  sound = valid;
  saline::Default_Data_Store store;
  load(store, data);
  auto view = store.setView({"A", "B"}, {0.25, 0.75});
  for (const auto &invalid : {json(), json::array(), json::object(),
       json{{"value", 0}}, json{{"value", -1}}, json{{"value", "2500"}},
       json{{"value", nullptr}}, json{{"value", 2500}, {"reference", 1}},
       json{{"value", 2500}, {"DOI", false}},
       json{{"value", 2500}, {"value_notes", nullptr}},
       json{{"value", 2500}, {"uncertainty_notes", 1}},
       json{{"value", 2500}, {"pct_uncertainty", -1}},
       json{{"value", 2500}, {"pct_uncertainty", "unknown"}},
       json{{"value", 2500}, {"range", {500}}},
       json{{"value", 2500}, {"range", {500, 1000, 1500}}},
       json{{"value", 2500}, {"range", {1000, 500}}},
       json{{"value", 2500}, {"range", {-1, 500}}},
       json{{"value", 2500}, {"range", {"500", 1000}}}}) {
    sound = invalid;
    try {
      load(store, data);
      FAIL() << "Accepted invalid speed-of-sound data: " << invalid.dump();
    } catch (const std::runtime_error &e) {
      EXPECT_NE(std::string::npos, std::string(e.what()).find("/speed_of_sound/"));
    }
    EXPECT_DOUBLE_EQ(2500, view.speedOfSound(900));
    EXPECT_EQ("unchanged", view.speedOfSound_ref());
    EXPECT_EQ(valid, json::parse(store.to_json_by_id(0, 0)).at("speed_of_sound"));
  }
}
