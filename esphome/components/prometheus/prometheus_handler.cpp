#include "prometheus_handler.h"
#include "esphome/core/application.h"
#include "esphome/components/climate/climate_mode.h"
#include "esphome/components/climate/climate_traits.h"

namespace esphome {
namespace prometheus {

void PrometheusHandler::handleRequest(AsyncWebServerRequest *req) {
  AsyncResponseStream *stream = req->beginResponseStream("text/plain", this->buffer_size_);

#ifdef USE_SENSOR
  this->sensor_type_(stream);
  for (auto *obj : App.get_sensors())
    this->sensor_row_(stream, obj);
#endif

#ifdef USE_BINARY_SENSOR
  this->binary_sensor_type_(stream);
  for (auto *obj : App.get_binary_sensors())
    this->binary_sensor_row_(stream, obj);
#endif

#ifdef USE_FAN
  this->fan_type_(stream);
  for (auto *obj : App.get_fans())
    this->fan_row_(stream, obj);
#endif

#ifdef USE_LIGHT
  this->light_type_(stream);
  for (auto *obj : App.get_lights())
    this->light_row_(stream, obj);
#endif

#ifdef USE_COVER
  this->cover_type_(stream);
  for (auto *obj : App.get_covers())
    this->cover_row_(stream, obj);
#endif

#ifdef USE_SWITCH
  this->switch_type_(stream);
  for (auto *obj : App.get_switches())
    this->switch_row_(stream, obj);
#endif

#ifdef USE_CLIMATE
  this->climate_type_(stream);
  for (auto *obj : App.get_climates())
    this->climate_row_(stream, obj);
#endif

  req->send(stream);
}

// Type-specific implementation
#ifdef USE_SENSOR
void PrometheusHandler::sensor_type_(AsyncResponseStream *stream) {
  stream->print(F("#TYPE esphome_sensor_value GAUGE\n"));
  stream->print(F("#TYPE esphome_sensor_failed GAUGE\n"));
}
void PrometheusHandler::sensor_row_(AsyncResponseStream *stream, sensor::Sensor *obj) {
  if (obj->is_internal())
    return;
  if (!isnan(obj->state)) {
    // We have a valid value, output this value
    stream->print(F("esphome_sensor_failed{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} 0\n"));
    // Data itself
    stream->print(F("esphome_sensor_value{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\",unit=\""));
    stream->print(obj->get_unit_of_measurement().c_str());
    stream->print(F("\"} "));
    stream->print(value_accuracy_to_string(obj->state, obj->get_accuracy_decimals()).c_str());
    stream->print('\n');
  } else {
    // Invalid state
    stream->print(F("esphome_sensor_failed{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} 1\n"));
  }
}
#endif

// Type-specific implementation
#ifdef USE_BINARY_SENSOR
void PrometheusHandler::binary_sensor_type_(AsyncResponseStream *stream) {
  stream->print(F("#TYPE esphome_binary_sensor_value GAUGE\n"));
  stream->print(F("#TYPE esphome_binary_sensor_failed GAUGE\n"));
}
void PrometheusHandler::binary_sensor_row_(AsyncResponseStream *stream, binary_sensor::BinarySensor *obj) {
  if (obj->is_internal())
    return;
  if (obj->has_state()) {
    // We have a valid value, output this value
    stream->print(F("esphome_binary_sensor_failed{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} 0\n"));
    // Data itself
    stream->print(F("esphome_binary_sensor_value{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} "));
    stream->print(obj->state);
    stream->print('\n');
  } else {
    // Invalid state
    stream->print(F("esphome_binary_sensor_failed{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} 1\n"));
  }
}
#endif

#ifdef USE_FAN
void PrometheusHandler::fan_type_(AsyncResponseStream *stream) {
  stream->print(F("#TYPE esphome_fan_value GAUGE\n"));
  stream->print(F("#TYPE esphome_fan_failed GAUGE\n"));
  stream->print(F("#TYPE esphome_fan_speed GAUGE\n"));
  stream->print(F("#TYPE esphome_fan_oscillation GAUGE\n"));
}
void PrometheusHandler::fan_row_(AsyncResponseStream *stream, fan::FanState *obj) {
  if (obj->is_internal())
    return;
  stream->print(F("esphome_fan_failed{id=\""));
  stream->print(obj->get_object_id().c_str());
  stream->print(F("\",name=\""));
  stream->print(obj->get_name().c_str());
  stream->print(F("\"} 0\n"));
  // Data itself
  stream->print(F("esphome_fan_value{id=\""));
  stream->print(obj->get_object_id().c_str());
  stream->print(F("\",name=\""));
  stream->print(obj->get_name().c_str());
  stream->print(F("\"} "));
  stream->print(obj->state);
  stream->print('\n');
  // Speed if available
  if (obj->get_traits().supports_speed()) {
    stream->print(F("esphome_fan_speed{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} "));
    stream->print(obj->speed);
    stream->print('\n');
  }
  // Oscillation if available
  if (obj->get_traits().supports_oscillation()) {
    stream->print(F("esphome_fan_oscillation{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} "));
    stream->print(obj->oscillating);
    stream->print('\n');
  }
}
#endif

#ifdef USE_LIGHT
void PrometheusHandler::light_type_(AsyncResponseStream *stream) {
  stream->print(F("#TYPE esphome_light_state GAUGE\n"));
  stream->print(F("#TYPE esphome_light_color GAUGE\n"));
  stream->print(F("#TYPE esphome_light_effect_active GAUGE\n"));
}
void PrometheusHandler::light_row_(AsyncResponseStream *stream, light::LightState *obj) {
  if (obj->is_internal())
    return;
  // State
  stream->print(F("esphome_light_state{id=\""));
  stream->print(obj->get_object_id().c_str());
  stream->print(F("\",name=\""));
  stream->print(obj->get_name().c_str());
  stream->print(F("\"} "));
  stream->print(obj->remote_values.is_on());
  stream->print(F("\n"));
  // Brightness and RGBW
  light::LightColorValues color = obj->current_values;
  float brightness, r, g, b, w;
  color.as_brightness(&brightness);
  color.as_rgbw(&r, &g, &b, &w);
  stream->print(F("esphome_light_color{id=\""));
  stream->print(obj->get_object_id().c_str());
  stream->print(F("\",name=\""));
  stream->print(obj->get_name().c_str());
  stream->print(F("\",channel=\"brightness\"} "));
  stream->print(brightness);
  stream->print(F("\n"));
  stream->print(F("esphome_light_color{id=\""));
  stream->print(obj->get_object_id().c_str());
  stream->print(F("\",name=\""));
  stream->print(obj->get_name().c_str());
  stream->print(F("\",channel=\"r\"} "));
  stream->print(r);
  stream->print(F("\n"));
  stream->print(F("esphome_light_color{id=\""));
  stream->print(obj->get_object_id().c_str());
  stream->print(F("\",name=\""));
  stream->print(obj->get_name().c_str());
  stream->print(F("\",channel=\"g\"} "));
  stream->print(g);
  stream->print(F("\n"));
  stream->print(F("esphome_light_color{id=\""));
  stream->print(obj->get_object_id().c_str());
  stream->print(F("\",name=\""));
  stream->print(obj->get_name().c_str());
  stream->print(F("\",channel=\"b\"} "));
  stream->print(b);
  stream->print(F("\n"));
  stream->print(F("esphome_light_color{id=\""));
  stream->print(obj->get_object_id().c_str());
  stream->print(F("\",name=\""));
  stream->print(obj->get_name().c_str());
  stream->print(F("\",channel=\"w\"} "));
  stream->print(w);
  stream->print(F("\n"));
  // Effect
  std::string effect = obj->get_effect_name();
  if (effect == "None") {
    stream->print(F("esphome_light_effect_active{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\",effect=\"None\"} 0\n"));
  } else {
    stream->print(F("esphome_light_effect_active{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\",effect=\""));
    stream->print(effect.c_str());
    stream->print(F("\"} 1\n"));
  }
}
#endif

#ifdef USE_COVER
void PrometheusHandler::cover_type_(AsyncResponseStream *stream) {
  stream->print(F("#TYPE esphome_cover_value GAUGE\n"));
  stream->print(F("#TYPE esphome_cover_failed GAUGE\n"));
}
void PrometheusHandler::cover_row_(AsyncResponseStream *stream, cover::Cover *obj) {
  if (obj->is_internal())
    return;
  if (!isnan(obj->position)) {
    // We have a valid value, output this value
    stream->print(F("esphome_cover_failed{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} 0\n"));
    // Data itself
    stream->print(F("esphome_cover_value{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} "));
    stream->print(obj->position);
    stream->print('\n');
    if (obj->get_traits().get_supports_tilt()) {
      stream->print(F("esphome_cover_tilt{id=\""));
      stream->print(obj->get_object_id().c_str());
      stream->print(F("\",name=\""));
      stream->print(obj->get_name().c_str());
      stream->print(F("\"} "));
      stream->print(obj->tilt);
      stream->print('\n');
    }
  } else {
    // Invalid state
    stream->print(F("esphome_cover_failed{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} 1\n"));
  }
}
#endif

#ifdef USE_SWITCH
void PrometheusHandler::switch_type_(AsyncResponseStream *stream) {
  stream->print(F("#TYPE esphome_switch_value GAUGE\n"));
  stream->print(F("#TYPE esphome_switch_failed GAUGE\n"));
}
void PrometheusHandler::switch_row_(AsyncResponseStream *stream, switch_::Switch *obj) {
  if (obj->is_internal())
    return;
  stream->print(F("esphome_switch_failed{id=\""));
  stream->print(obj->get_object_id().c_str());
  stream->print(F("\",name=\""));
  stream->print(obj->get_name().c_str());
  stream->print(F("\"} 0\n"));
  // Data itself
  stream->print(F("esphome_switch_value{id=\""));
  stream->print(obj->get_object_id().c_str());
  stream->print(F("\",name=\""));
  stream->print(obj->get_name().c_str());
  stream->print(F("\"} "));
  stream->print(obj->state);
  stream->print('\n');
}
#endif

#ifdef USE_CLIMATE
void PrometheusHandler::climate_type_(AsyncResponseStream *stream) {
  stream->print(F("#TYPE esphome_climate_mode GAUGE\n"));
  stream->print(F("#TYPE esphome_climate_failed GAUGE\n"));
}

void PrometheusHandler::climate_row_(AsyncResponseStream *stream, climate::Climate *obj) {
  const char *modes_values[6] = {"off", "auto", "cool", "heat", "fan_only", "dry"};
  const char *action_values[7] = {"off", "", "cooling", "heating", "idle", "drying", "fan"};
  const char *fan_mode_values[9] = {"on", "off", "auto", "low", "medium", "high", "middle", "focus", "diffuse"};
  const char *swing_mode_values[4] = {"off", "both", "vertical", "horuzontal"};
  const int8_t accuracy_decimals = obj->get_traits().get_temperature_accuracy_decimals();

  if (obj->is_internal())
    return;
  if (!isnan(obj->target_temperature)) {
    stream->print(F("esphome_climate_failed{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} 0\n"));

    stream->print(F("esphome_climate_mode{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} "));
    stream->print((uint) obj->mode);
    stream->print('\n');

    for (uint8_t i = 0; i < 6; i++) {
      if (obj->get_traits().supports_mode(climate::ClimateMode(i))) {
        stream->print(F("esphome_climate_mode{id=\""));
        stream->print(obj->get_object_id().c_str());
        stream->print(F("\",name=\""));
        stream->print(obj->get_name().c_str());
        stream->print(F("\",mode=\""));
        stream->print(modes_values[i]);
        if (obj->mode == climate::ClimateMode(i))
          stream->print(F("\"} 1\n"));
        else
          stream->print(F("\"} 0\n"));
      }
    }

    if (obj->get_traits().get_supports_current_temperature()) {
      stream->print(F("#TYPE esphome_climate_current_temperature GAUGE\n"));
      stream->print(F("esphome_climate_current_temperature{id=\""));
      stream->print(obj->get_object_id().c_str());
      stream->print(F("\",name=\""));
      stream->print(obj->get_name().c_str());
      stream->print(F("\",unit=\"°C\"} "));
      stream->print(value_accuracy_to_string(obj->current_temperature, accuracy_decimals).c_str());
      stream->print('\n');
    }
    if (obj->get_traits().get_supports_two_point_target_temperature()) {
      stream->print(F("#TYPE esphome_climate_target_temperature_high GAUGE\n"));
      stream->print(F("esphome_climate_target_temperature_high{id=\""));
      stream->print(obj->get_object_id().c_str());
      stream->print(F("\",name=\""));
      stream->print(obj->get_name().c_str());
      stream->print(F("\",unit=\"°C\"} "));
      stream->print(value_accuracy_to_string(obj->target_temperature_high, accuracy_decimals).c_str());
      stream->print('\n');
      stream->print(F("esphome_climate_target_temperature_low{id=\""));
      stream->print(obj->get_object_id().c_str());
      stream->print(F("\",name=\""));
      stream->print(obj->get_name().c_str());
      stream->print(F("\",unit=\"°C\"} "));
      stream->print(value_accuracy_to_string(obj->target_temperature_low, accuracy_decimals).c_str());
      stream->print('\n');
    } else {
      stream->print(F("#TYPE esphome_climate_target_temperature GAUGE\n"));
      stream->print(F("esphome_climate_target_temperature{id=\""));
      stream->print(obj->get_object_id().c_str());
      stream->print(F("\",name=\""));
      stream->print(obj->get_name().c_str());
      stream->print(F("\",unit=\"°C\"} "));
      stream->print(value_accuracy_to_string(obj->target_temperature, accuracy_decimals).c_str());
      stream->print('\n');
    }
    if (obj->get_traits().get_supports_action()) {
      stream->print(F("#TYPE esphome_climate_action GAUGE\n"));
      stream->print(F("esphome_climate_action{id=\""));
      stream->print(obj->get_object_id().c_str());
      stream->print(F("\",name=\""));
      stream->print(obj->get_name().c_str());
      stream->print(F("\"} "));
      stream->print((uint) obj->action);
      stream->print('\n');

      for (uint8_t i = 0; i < 7; i++) {
        if (i == 1)
          continue;
        stream->print(F("esphome_climate_action{id=\""));
        stream->print(obj->get_object_id().c_str());
        stream->print(F("\",name=\""));
        stream->print(obj->get_name().c_str());
        stream->print(F("\",action=\""));
        stream->print(action_values[i]);
        if (obj->action == climate::ClimateAction(i))
          stream->print(F("\"} 1\n"));
        else
          stream->print(F("\"} 0\n"));
      }
    }
    if (obj->get_traits().get_supports_away()) {
      stream->print(F("#TYPE esphome_climate_away GAUGE\n"));
      stream->print(F("esphome_climate_away{id=\""));
      stream->print(obj->get_object_id().c_str());
      stream->print(F("\",name=\""));
      stream->print(obj->get_name().c_str());
      stream->print(F("\"} "));
      if (obj->away) {
        stream->print("1\n");
      } else {
        stream->print("0\n");
      }
    }

    obj->fan_mode = climate::ClimateFanMode(1);
    if (obj->get_traits().get_supports_fan_modes()) {
      stream->print(F("#TYPE esphome_climate_fan_mode GAUGE\n"));
      stream->print(F("esphome_climate_fan_mode{id=\""));
      stream->print(obj->get_object_id().c_str());
      stream->print(F("\",name=\""));
      stream->print(obj->get_name().c_str());
      stream->print(F("\"} "));
      stream->print((uint) obj->fan_mode);
      stream->print('\n');

      for (uint8_t i = 0; i < 9; i++) {
        if (obj->get_traits().supports_fan_mode(climate::ClimateFanMode(i))) {
          stream->print(F("esphome_climate_fan_mode{id=\""));
          stream->print(obj->get_object_id().c_str());
          stream->print(F("\",name=\""));
          stream->print(obj->get_name().c_str());
          stream->print(F("\",mode=\""));
          stream->print(fan_mode_values[i]);
          if (obj->fan_mode == climate::ClimateFanMode(i))
            stream->print(F("\"} 1\n"));
          else
            stream->print(F("\"} 0\n"));
        }
      }
    }

    obj->swing_mode = climate::ClimateSwingMode(1);
    if (obj->get_traits().get_supports_swing_modes()) {
      stream->print(F("#TYPE esphome_climate_swing_mode GAUGE\n"));
      stream->print(F("esphome_climate_swing_mode{id=\""));
      stream->print(obj->get_object_id().c_str());
      stream->print(F("\",name=\""));
      stream->print(obj->get_name().c_str());
      stream->print(F("\"} "));
      stream->print((uint) obj->swing_mode);
      stream->print('\n');

      for (uint8_t i = 0; i < 4; i++) {
        if (obj->get_traits().supports_swing_mode(climate::ClimateSwingMode(i))) {
          stream->print(F("esphome_climate_fan_mode{id=\""));
          stream->print(obj->get_object_id().c_str());
          stream->print(F("\",name=\""));
          stream->print(obj->get_name().c_str());
          stream->print(F("\",mode=\""));
          stream->print(swing_mode_values[i]);
          if (obj->swing_mode == climate::ClimateSwingMode(i))
            stream->print(F("\"} 1\n"));
          else
            stream->print(F("\"} 0\n"));
        }
      }
    }
  } else {
    // Invalid state
    stream->print(F("esphome_climate_failed{id=\""));
    stream->print(obj->get_object_id().c_str());
    stream->print(F("\",name=\""));
    stream->print(obj->get_name().c_str());
    stream->print(F("\"} 1\n"));
  }
}
#endif

}  // namespace prometheus
}  // namespace esphome
