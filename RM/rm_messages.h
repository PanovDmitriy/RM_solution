#pragma once

#include "singleton.h"

namespace rm // rule machine
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

        base_messages& operator=(const base_messages& m)
        {
            const_cast<std::string&>(true_nenabled) = m.true_nenabled;
            const_cast<std::string&>(false_curr_state_null) = m.false_curr_state_null;
            const_cast<std::string&>(false_curr_state_nfaund) = m.false_curr_state_nfaund;
            const_cast<std::string&>(true_reject) = m.true_reject;
            const_cast<std::string&>(false_target_state_null) = m.false_target_state_null;
            const_cast<std::string&>(true_curr_state_final) = m.true_curr_state_final;
            const_cast<std::string&>(true_ok) = m.true_ok;

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
            "Ok"
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
            "Ok"
            }
        {
        }
    };

    template <typename TMessages>
    struct sgt_messages :
        singleton<sgt_messages<TMessages>>
    {
        const TMessages msgs;

        void set(const TMessages& msgs_)
        {
            const_cast<TMessages&>(msgs) = msgs_;
        }
    };

} //namespace rm