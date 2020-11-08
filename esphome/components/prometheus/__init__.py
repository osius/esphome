import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID, CONF_BUFFER_SIZE
from esphome.components.web_server_base import CONF_WEB_SERVER_BASE_ID
from esphome.components import web_server_base

AUTO_LOAD = ['web_server_base']

prometheus_ns = cg.esphome_ns.namespace('prometheus')
PrometheusHandler = prometheus_ns.class_('PrometheusHandler', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PrometheusHandler),
    cv.GenerateID(CONF_WEB_SERVER_BASE_ID): cv.use_id(web_server_base.WebServerBase),
    cv.Optional(CONF_BUFFER_SIZE, default=1460): cv.templatable(cv.int_range(min=1024, max=3072)),

}).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    paren = yield cg.get_variable(config[CONF_WEB_SERVER_BASE_ID])

    var = cg.new_Pvariable(config[CONF_ID], paren)
    yield cg.register_component(var, config)

    cg.add(var.set_buffer_size(config[CONF_BUFFER_SIZE]))
