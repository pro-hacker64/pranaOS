/*
 * Copyright (c) 2021, krishpranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

// includes
#include <libabi/Result.h>
#include <libjson/Json.h>
#include <libmath/MinMax.h>
#include <string.h>
#include "devfs/DevicesInfo.h"
#include "system/devices/Devices.h"
#include "system/interrupts/Interupts.h"
#include "system/node/Handle.h"
#include "system/scheduling/Scheduler.h"

FsDeviceInfo::FsDeviceInfo() : FsNode(J_FILE_TYPE_DEVICE)
{
}

JResult FsDeviceInfo::open(FsHandle &handle)
{
    Json::Value::Array root{};

    device_iterate([&](RefPtr<Device> device) {
        Json::Value::Object device_object{};

        UNUSED(device);

        device_object["name"] = device->name().cstring();
        device_object["path"] = device->path().cstring();
        device_object["address"] = device->address().as_static_cstring();
        device_object["interrupt"] = (int64_t)device->interrupt();
        device_object["refcount"] = (int64_t)device->refcount();

        auto *driver = driver_for(device->address());
        if (driver)
        {
            device_object["description"] = driver->name();
        }

        root.push_back(device_object);

        return Iteration::CONTINUE;
    });

    auto str = Json::stringify(root);
    handle.attached = str.storage().give_ref();
    handle.attached_size = reinterpret_cast<StringStorage *>(handle.attached)->size();

    return SUCCESS;
}
