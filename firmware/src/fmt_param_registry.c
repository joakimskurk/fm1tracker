/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_param_registry.h"

#include <stddef.h>

static const FmtParamInfo fmt_param_registry[] = {
#define FMT_PARAM_INFO(symbol, value, label_text) \
    { (FmtParamId)(value), FMT_PARAM_FLAG_LOCKABLE, (label_text) },
    FMT_PARAM_REGISTRY(FMT_PARAM_INFO)
#undef FMT_PARAM_INFO
};

uint8_t fmt_param_namespace(FmtParamId id)
{
    if (id == FMT_PARAM_NONE)
        return FMT_PARAM_NAMESPACE_NONE;
    if (id >= FMT_PARAM_COMMON_FIRST && id <= FMT_PARAM_COMMON_LAST)
        return FMT_PARAM_NAMESPACE_COMMON;
    if (id >= FMT_PARAM_8BIT_FIRST && id <= FMT_PARAM_8BIT_LAST)
        return FMT_PARAM_NAMESPACE_8BIT;
    if (id >= FMT_PARAM_16BIT_FIRST && id <= FMT_PARAM_16BIT_LAST)
        return FMT_PARAM_NAMESPACE_16BIT;
    if (id >= FMT_PARAM_TRACK_FIRST && id <= FMT_PARAM_TRACK_LAST)
        return FMT_PARAM_NAMESPACE_TRACK;
    if (id >= FMT_PARAM_CHANNEL_FX_FIRST && id <= FMT_PARAM_CHANNEL_FX_LAST)
        return FMT_PARAM_NAMESPACE_CHANNEL_FX;
    if (id >= FMT_PARAM_GLOBAL_FIRST && id <= FMT_PARAM_GLOBAL_LAST)
        return FMT_PARAM_NAMESPACE_GLOBAL;
    if (id >= FMT_PARAM_RESERVED_FIRST && id <= FMT_PARAM_RESERVED_LAST)
        return FMT_PARAM_NAMESPACE_RESERVED;
    return FMT_PARAM_NAMESPACE_INVALID;
}

uint32_t fmt_param_registry_count(void)
{
    return (uint32_t)(sizeof fmt_param_registry / sizeof fmt_param_registry[0]);
}

const FmtParamInfo *fmt_param_registry_at(uint32_t index)
{
    if (index >= fmt_param_registry_count())
        return (const FmtParamInfo *)0;
    return &fmt_param_registry[index];
}

const FmtParamInfo *fmt_param_lookup(FmtParamId id)
{
    uint32_t i;

    for (i = 0u; i < fmt_param_registry_count(); i++) {
        if (fmt_param_registry[i].id == id)
            return &fmt_param_registry[i];
    }
    return (const FmtParamInfo *)0;
}

int fmt_param_is_assigned(FmtParamId id)
{
    return fmt_param_lookup(id) != (const FmtParamInfo *)0;
}

int fmt_param_is_lockable(FmtParamId id)
{
    const FmtParamInfo *info = fmt_param_lookup(id);
    return info && (info->flags & FMT_PARAM_FLAG_LOCKABLE) != 0u;
}
