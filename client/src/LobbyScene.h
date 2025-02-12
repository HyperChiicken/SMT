/**
 * @file client/src/LobbyScene.h
 * @ingroup client
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Lobby scene.
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

#ifndef LIBCLIENT_SRC_LOBBYSCENE_H
#define LIBCLIENT_SRC_LOBBYSCENE_H

// Qt Includes
#include "ui_LobbyScene.h"

// libclient Includes
#include <ClientManager.h>

namespace packets {

class PacketLobbyCharacterList;

}  // namespace packets

namespace logic {

class LogicWorker;

}  // namespace logic

namespace game {

class GameWorker;

/**
 * Scene to present the user with the lobby (character list).
 */
class LobbyScene : public QWidget, public logic::ClientManager {
  Q_OBJECT

 public:
  /**
   * Construct the lobby scene.
   * @param pWorker The GameWorker for the UI.
   * @param pParent Parent Qt widget for the dialog.
   */
  LobbyScene(GameWorker *pWorker, QWidget *pParent = nullptr);

  /**
   * Cleanup the scene.
   */
  ~LobbyScene() override;

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
   * Update the UI after selecting a character.
   */
  void selectionChanged();

  /**
   * Start game button was clicked.
   */
  void startGame();

 private:
  /**
   * Handle Character List Update message.
   * @param pMessage Client message to process.
   */
  bool HandleCharacterListUpdate(
      const libcomp::Message::MessageClient *pMessage);

  /**
   * Handle the authentication reply.
   * @param pMessage Client message to process.
   */
  bool HandleConnectedToChannel(
      const libcomp::Message::MessageClient *pMessage);

  /// Pointer to the LogicWorker.
  logic::LogicWorker *mLogicWorker;

  /// Pointer to the GameWorker.
  GameWorker *mGameWorker;

  /// Character list data.
  std::shared_ptr<packets::PacketLobbyCharacterList> mCharacterList;

  /// UI for this scene.
  Ui::LobbyScene ui;
};

}  // namespace game

#endif  // LIBCLIENT_SRC_LOBBYSCENE_H
