#pragma once

#include "singleton.h"

namespace Machines // rule machine
{
    struct base_messages
    {
        const std::string true_nenabled;
        const std::string false_curr_state_null;
        const std::string false_curr_state_nfaund;
        const std::string true_reject;
        const std::string false_target_state_null;
        const std::string true_curr_state_final;
        const std::string true_ok;
        const std::string param_state_is_null;
        const std::string init_state_is_already_set;
        const std::string final_state_is_already_set;
        const std::string false_riser_is_null;

        base_messages& operator=(const base_messages& m)
        {
            const_cast<std::string&>(true_nenabled) = m.true_nenabled;
            const_cast<std::string&>(false_curr_state_null) = m.false_curr_state_null;
            const_cast<std::string&>(false_curr_state_nfaund) = m.false_curr_state_nfaund;
            const_cast<std::string&>(true_reject) = m.true_reject;
            const_cast<std::string&>(false_target_state_null) = m.false_target_state_null;
            const_cast<std::string&>(true_curr_state_final) = m.true_curr_state_final;
            const_cast<std::string&>(true_ok) = m.true_ok;
            const_cast<std::string&>(param_state_is_null) = m.param_state_is_null;
            const_cast<std::string&>(init_state_is_already_set) = m.init_state_is_already_set;
            const_cast<std::string&>(final_state_is_already_set) = m.final_state_is_already_set;
            const_cast<std::string&>(false_riser_is_null) = m.false_riser_is_null;

            return *this;
        }
    };

    struct msgs_en :
        public base_messages
    {
        msgs_en() :
            base_messages
            {
            "Event reject. sm status: not enabled",
            "Current state is null",
            "Current state is not faund",
            "State reject event or Link guard condition reject",
            "Target state is null",
            "Current state is Final. State machine is disable",
            "Ok",
            "Parameter state ptr is null",
            "Initial state is already set",
            "Final state is already set",
            "Riser is null"
        }
        {
        }
    };

    struct msgs_ru :
        base_messages
    {
        msgs_ru() :
            base_messages
            {
            "Событие отвергнуто. Статус автомата: не включен",
            "Текущее состояние NULL",
            "Текущее состояние не найдено",
            "Состояние не приняло событие или сторожевое условие перехода не выполнено",
            "Целевое состояние NULL",
            "Текущее состояние Final. Автомат выключен",
            "Ok",
            "Параметр state = NULL",
            "Начальное состояние уже задано",
            "Конечное состояние уже задано",
            "Инициатор события не задан"
        }
        {
        }
    };

    using sgt_messages = singleton_const_data<base_messages>;
    inline auto msg_()
    {
        return sgt_messages <base_messages>::get_instance().msgs;
    }

    //template <typename TMessages>
    //struct sgt_messages :
    //    singleton<sgt_messages<TMessages>>
    //{
    //    const TMessages msgs;

    //    void set(const TMessages& msgs_)
    //    {
    //        const_cast<TMessages&>(msgs) = msgs_;
    //    }
    //};

} //namespace Machines