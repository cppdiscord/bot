#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <iostream>
#include <dpp/dpp.h>

#define COLOR_DEFAULT 0x004482

namespace msgHandler
{
    /**
     * @brief Send a suggestion
     * @param bot
     * @param message_create_t event
     */
    void sendSuggestion(dpp::cluster& bot, const dpp::message_create_t& event);

    namespace btns {

        /**
         * @brief Deletes a suggestion
         * @param bot
         * @param button_click_t event
         */
        void deleteSuggestionBtn(dpp::cluster& bot, const dpp::button_click_t& event);

        /**
         * @brief Edits a suggestion
         * @param bot
         * @param button_click_t event
         */
        void editSuggestionBtn(dpp::cluster& bot, const dpp::button_click_t& event);

    }

    namespace modalForms {
        /**
         * @brief Edit suggestion modal
         * @param bot
         * @param form_submit_t event
         */
        void editSuggetionModal(dpp::cluster& bot, const dpp::form_submit_t& event);
    }
}

#endif // MESSAGEMANAGER_H
