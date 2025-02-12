/**
 * @file client/src/ChannelScene.h
 * @ingroup client
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Channel scene.
 *
 * This file is part of the COMP_hack Test Client (client).
 *
 * Copyright (C) 2012-2020 COMP_hack Team <compomega@tutanota.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBCLIENT_SRC_CHANNELSCENE_H
#define LIBCLIENT_SRC_CHANNELSCENE_H

// Qt Includes
#include "ui_ChannelScene.h"

// libclient Includes
#include <ClientManager.h>

namespace logic {

class LogicWorker;

}  // namespace logic

namespace game {

class GameWorker;

/**
 * Scene to present the user with the channel.
 */
class ChannelScene : public QWidget, public logic::ClientManager {
  Q_OBJECT

 public:
  /**
   * Construct the channel scene.
   * @param pWorker The GameWorker for the UI.
   * @param pParent Parent Qt widget for the dialog.
   */
  ChannelScene(GameWorker *pWorker, QWidget *pParent = nullptr);

  /**
   * Cleanup the scene.
   */
  ~ChannelScene() override;

  /**
   * Process a client message.
   * @param pMessage Client message to process.
   */
  bool ProcessClientMessage(const libcomp::Message::MessageClient *pMessage);

 protected:
  /**
   * Handle a close event on the scene.
   * @param pEvent Close event to handle.
   */
  void closeEvent(QCloseEvent *pEvent) override;

 private slots:
  /**
   * Backup the account.
   */
  void backupAccount();

 private:
  /**
   * Handle account dump status message.
   * @param pMessage Client message to process.
   */
  bool HandleAccountDumpStatus(const libcomp::Message::MessageClient *pMessage);

  /// Pointer to the LogicWorker.
  logic::LogicWorker *mLogicWorker;

  /// Pointer to the GameWorker.
  GameWorker *mGameWorker;

  /// UI for this scene.
  Ui::ChannelScene ui;
};

}  // namespace game

#endif  // LIBCLIENT_SRC_CHANNELSCENE_H
