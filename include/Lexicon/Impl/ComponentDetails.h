#pragma once

#define DEF_USING_IMPL(mc_func, mc_type) \
using mc_type::mc_func; \


#define DEFINE_COMPONENT_TYPE(mc_type) static constexpr ComponentType COMPONENT_TYPE = mc_type;\
ComponentType GetComponentType() const override { return COMPONENT_TYPE; }

#define DEFINE_COMPONENT_OFFSET(mc_type,...) DEFINE_COMPONENT_TYPE(mc_type) \
static constexpr ComponentType SCRIPT_TYPE = __VA_OPT__(true ? __VA_ARGS__ : )mc_type; \
uint16_t GetComponentOffset() const override { assert(SCRIPT_TYPE < ComponentType::kScriptedMax); return SCRIPT_TYPE; }

