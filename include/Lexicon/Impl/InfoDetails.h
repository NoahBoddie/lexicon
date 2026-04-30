
#define DEFINE_INFO_TYPE(mc_type) static constexpr InfoType INFO_TYPE = mc_type;\
InfoType GetInfoType() const override { return INFO_TYPE; }

#define DEFINE_INFO_OFFSET(mc_type,...) DEFINE_INFO_TYPE(mc_type) \
static constexpr InfoType SCRIPT_INFO_TYPE = __VA_OPT__(true ? __VA_ARGS__ : )mc_type; \
uint16_t GetInfoOffset() const override { assert(SCRIPT_INFO_TYPE < InfoType::kScriptedMax); return SCRIPT_INFO_TYPE; }

