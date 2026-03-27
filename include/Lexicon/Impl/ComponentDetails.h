#pragma once

#define DEF_USING_IMPL(mc_func, mc_type) \
using mc_type::mc_func; \


#define DEFINE_COMPONENT_TYPE(mc_type) static constexpr ComponentType COMPONENT_TYPE = mc_type;\
ComponentType GetComponentType() const override { return COMPONENT_TYPE; }

#define DEFINE_COMPONENT_OFFSET(mc_type) DEFINE_COMPONENT_TYPE(mc_type) uint16_t GetComponentOffset() const override \
{ assert(COMPONENT_TYPE < ComponentType::kScriptedMax); return COMPONENT_TYPE; }

