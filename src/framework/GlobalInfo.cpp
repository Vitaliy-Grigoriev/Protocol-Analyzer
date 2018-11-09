// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include "../../include/framework/System.hpp"  // system::allocMemoryForArray.
#include "../../include/framework/GlobalInfo.hpp"


namespace analyzer::framework::storage
{
    // Function that returns size of the selected module callbacks.
    uint16_t GlobalInfo::GetModuleCallbacksSize (const uint16_t module) noexcept
    {
        switch (module)
        {
            case modules::MODULE_SOCKET: return callbacks::MODULE_SOCKET_TYPES_SIZE;
            default:
                LOG_ERROR("getModuleCallbacksSize: Incorrect input module type - ", module, '.');
                return 0;
        }
    }

    // Constructor of GlobalInfo class.
    GlobalInfo::GlobalInfo(void) noexcept
    {

    }

    // Method that returns the instance of the GlobalInfo singleton class.
    GlobalInfo& GlobalInfo::Instance(void) noexcept
    {
        static GlobalInfo instance;
        return instance;
    }

    // Method that sets callback functor of selected type.
    bool GlobalInfo::SetCallback (callbacks::BaseCallbackFunctor* const functor, const uint16_t module, const uint16_t callback) noexcept
    {
        if (module >= modules::FRAMEWORK_MODULE_TYPES_SIZE) {
            LOG_ERROR("GlobalInfo.SetCallback: Incorrect input module type - ", module, '.');
            return false;
        }

        const uint16_t callbacksSize = GetModuleCallbacksSize(module);
        if (callback < callbacksSize)
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::system_error& /*err*/) {
                return false;
            }

            if (callbacks[module] == nullptr) {
                callbacks[module] = system::allocMemoryForArray<std::unique_ptr<callbacks::BaseCallbackFunctor>>(callbacksSize);
            }

            if (callbacks[module][callback] != nullptr) {
                LOG_WARNING("GlobalInfo.SetCallback: Callback functor '", callback, "' overwritten, module - ", module, '.');
            }
            callbacks[module][callback].reset(functor);
            LOG_TRACE("GlobalInfo.SetCallback: Callback functor '", callback, "' is set successfully, module - ", module, '.');
            return true;
        }
        LOG_ERROR("GlobalInfo.SetCallback: Incorrect input callback type - ", callback, '.');
        return false;
    }


    std::once_flag flag;

    // Method that returns reference of the SystemNetworkConfiguration class with filled information.
    system::SystemNetworkConfiguration& GlobalInfo::GetNetworkInformation(void) noexcept
    {
        std::call_once(flag, [&]() { if (networkConfiguration.Initialize() == false) { std::terminate(); } });
        return networkConfiguration;
    }

}  // namespace storage.
