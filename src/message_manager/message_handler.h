#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <iostream>
#include <dpp/dpp.h>

#define COLOR_DEFAULT 0x004482

namespace MsgHandler
{
    /**
     * @brief Creates a suggestion
     * @param bot
     * @param message_create_t event
     */
    void createSuggestion(dpp::cluster& bot, const dpp::message_create_t& event);

    namespace Btns {

        /**
         * @brief Deletes a suggestion
         * @param bot
         * @param button_click_t event
         */
        void deleteSuggestionBtn(dpp::cluster& bot, const dpp::button_click_t& event);

        /**
         * @brief Shows the modal to edit an suggestion
         * @param bot
         * @param button_click_t event
         */
        void editSuggestionBtn(dpp::cluster& bot, const dpp::button_click_t& event);

    }

    namespace ModalForms {
        /**
         * @brief Edit suggestion modal
         * @param bot
         * @param form_submit_t event
         */
        void showSuggestionEditModal(dpp::cluster& bot, const dpp::form_submit_t& event);
    }
}

#endif // MESSAGEMANAGER_H
