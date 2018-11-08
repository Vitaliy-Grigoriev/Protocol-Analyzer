// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_GLOBAL_INFO_HPP
#define PROTOCOL_ANALYZER_GLOBAL_INFO_HPP

#include <array>  // std::array.
#include <memory>  // std::unique_ptr.

#include "Log.hpp"  // log::Logger.
#include "FrameworkModuleTypes.hpp"  // modules::FRAMEWORK_MODULE_TYPES.
#include "FrameworkCallbackFunctors.hpp"  // callbacks::BaseCallbackFunctor.
#include "SystemNetworkConfiguration.hpp"  // system::SystemNetworkConfiguration.


namespace analyzer::framework::storage
{
    /**
     * @class GlobalInfo   GlobalInfo.hpp   "include/framework/GlobalInfo.hpp"
     * @brief This class defined the interface for storing global information through the framework.
     */
    class GlobalInfo
    {
    private:
        /**
         * @brief Mutex value for thread-safe class working.
         */
        std::mutex mutex = { };
        /**
         * @brief Variable that consist of various global callbacks.
         */
        std::array<std::unique_ptr<std::unique_ptr<callbacks::BaseCallbackFunctor>[]>, modules::FRAMEWORK_MODULE_TYPES_SIZE> callbacks = { };
        /**
         * @brief Class that stores information about system interfaces and routes.
         */
        system::SystemNetworkConfiguration networkConfiguration;


        /**
         * @brief Function that returns size of the selected module callbacks.
         *
         * @param [in] module - Type of framework module (FRAMEWORK_MODULE_TYPES).
         * @return Size of the selected module callbacks.
         */
        static uint16_t GetModuleCallbacksSize (uint16_t /*module*/) noexcept;

    public:
        GlobalInfo (GlobalInfo &&) = delete;
        GlobalInfo (const GlobalInfo &) = delete;
        GlobalInfo & operator= (GlobalInfo &&) = delete;
        GlobalInfo & operator= (const GlobalInfo &) = delete;

        /**
         * @brief Constructor of GlobalInfo class.
         */
        GlobalInfo(void) noexcept;

        /**
         * @brief Method that returns the instance of the GlobalInfo singleton class.
         *
         * @return The instance of GlobalInfo singleton class.
         */
        static GlobalInfo & Instance(void) noexcept;

        /**
         * @brief Method that returns constant reference of the SystemNetworkConfiguration class with filled information.
         * @return Reference of the SystemNetworkConfiguration class.
         */
        system::SystemNetworkConfiguration & GetNetworkInformation(void) noexcept;

        /**
         * @brief Method that returns pointer to callback functor of selected type.
         *
         * @tparam [in,out] Type - Functor type to which result will be converted.
         * @param [in] module - Type of framework module (FRAMEWORK_MODULE_TYPES).
         * @param [in] callback - Type of module callback (MODULE_***_TYPES).
         * @return Pointer to the BaseCallbackFunctor functor of selected type if it exists, otherwise - nullptr.
         */
        template <typename Type, typename = std::enable_if_t<std::is_base_of_v<callbacks::BaseCallbackFunctor, Type>>>
        Type* GetCallback (const uint16_t module, const uint16_t callback) const noexcept
        {
            if (module >= modules::FRAMEWORK_MODULE_TYPES_SIZE) {
                LOG_ERROR("GlobalInfo.GetCallback: Incorrect input module type - ", module, '.');
                return nullptr;
            }
            if (callbacks[module] == nullptr) {
                return nullptr;
            }

            if (callback < GetModuleCallbacksSize(module)) {
                return dynamic_cast<Type*>(callbacks[module][callback].get());
            }
            LOG_ERROR("GlobalInfo.GetCallback: Incorrect input callback type - ", callback, '.');
            return nullptr;
        }

        /**
         * @brief Method that sets callback functor of selected type.
         *
         * @param [in] functor - Pointer to callback functor.
         * @param [in] module - Type of framework module (FRAMEWORK_MODULE_TYPES).
         * @param [in] callback - Type of module callback (MODULE_***_TYPES).
         * @return TRUE - if callback functor is set successfully, otherwise - FALSE.
         *
         * @note This method is thread-safe.
         */
        bool SetCallback (callbacks::BaseCallbackFunctor * /*functor*/, uint16_t /*module*/, uint16_t /*callback*/) noexcept;

        /**
         * @brief Default destructor of GlobalInfo class.
         */
        ~GlobalInfo(void) = default;

    };



    /*********************************************** Global Variables ***********************************************/

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"

    /**
     * @brief Global variable that consist of the instance of the GlobalInfo singleton class.
     */
    inline static GlobalInfo& GI = GlobalInfo::Instance();

    /**
     * @brief Global variable that consist of the start time of Protocol Analyzer Framework.
     */
    inline const std::chrono::time_point<std::chrono::system_clock> ApplicationStartTime = std::chrono::system_clock::now();

#pragma clang diagnostic pop

    /*********************************************** Global Variables ***********************************************/

}  // namespace storage.

#endif  // PROTOCOL_ANALYZER_GLOBAL_INFO_HPP
