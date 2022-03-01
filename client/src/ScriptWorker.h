/**
 * @file client/src/ScriptWorker.h
 * @ingroup client
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Script worker to run automated client scripts.
 *
 * This file is part of the COMP_hack Test Client (client).
 *
 * Copyright (C) 2012-2021 COMP_hack Team <compomega@tutanota.com>
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

#ifndef LIBCLIENT_SRC_SCRIPTWORKER_H
#define LIBCLIENT_SRC_SCRIPTWORKER_H

// libcomp Includes
#include <EnumMap.h>
#include <Object.h>
#include <Packet.h>
#include <Worker.h>

// libhack Includes
#include <ScriptEngine.h>

// client Includes
#include <ClientManager.h>

namespace logic {

class LogicWorker;
class MessageConnectToLobby;

}  // namespace logic

namespace script {
//
// Forward declaration of managers.
//
/// @todo Add some?

/**
 * Worker for client<==>server test scripts.
 */
class ScriptWorker : public libcomp::Worker,
                     public libcomp::Manager,
                     public logic::ClientManager,
                     public std::enable_shared_from_this<ScriptWorker> {
 public:
  /**
   * Create a new worker.
   */
  ScriptWorker();

  /**
   * Cleanup the worker.
   */
  virtual ~ScriptWorker();

  /**
   * Get the different types of messages handled by the manager.
   * @return List of message types handled by the manager
   */
  std::list<libcomp::Message::MessageType> GetSupportedTypes() const override;

  /**
   * Process a message from the queue.
   * @param pMessage Message to be processed
   * @return true on success, false on failure
   */
  bool ProcessMessage(const libcomp::Message::Message *pMessage) override;

  /**
   * Wait for a message to enter the queue then handle it
   * with the appropriate @ref Manager configured for the
   * worker.
   * @param pMessageQueue Queue to check for messages
   */
  void Run(libcomp::MessageQueue<libcomp::Message::Message *> *pMessageQueue)
      override;

  /**
   * Signal that the worker should shutdown by sending a
   * @ref Message::Shutdown.
   */
  void Shutdown() override;

  /**
   * Send a message to the ScriptWorker message queue.
   * @param pMessage Message to send to the ScriptWorker.
   * @returns true if the message was sent; false otherwise.
   */
  bool SendToScript(libcomp::Message::Message *pMessage);

  /**
   * Send a message to the LogicWorker message queue for the given client.
   * @param clientUUID UUID of the client LogicWorker to send the message to.
   * @param pMessage Message to send to the LogicWorker.
   * @returns true if the message was sent; false otherwise.
   */
  bool SendToClient(const libobjgen::UUID &uuid,
                    libcomp::Message::Message *pMessage);

  /**
   * Process a client message.
   * @param pMessage Client message to process.
   */
  bool ProcessClientMessage(const libcomp::Message::MessageClient *pMessage);

  /**
   * Set the callback function to be called when a new client is created.
   * @param callback Callback function to call.
   * @note This is called in the context of the script worker thread!
   */
  void SetCreateClientCallback(
      std::function<void(ScriptWorker *, logic::LogicWorker *)> callback);

  /**
   * Set the callback function to be called when a client is deleted.
   * @param callback Callback function to call.
   * @note This is called in the context of the script worker thread!
   */
  void SetDeleteClientCallback(
      std::function<void(ScriptWorker *, logic::LogicWorker *)> callback);

  /**
   * Set the callback function to be called when a script has been run.
   * @param callback Callback function to call.
   * @note This is called in the context of the script worker thread!
   */
  void SetScriptCallback(
      std::function<void(ScriptWorker *, const libcomp::String &, bool)>
          callback);

  /**
   * Create a new client.
   * @param friendlyName Friendly name for the client.
   * @note This is not thread safe! This is here to be called by scripts ONLY.
   * @return Handle to the client created (or null on failure).
   */
  logic::LogicWorker *CreateClient(const libcomp::String &friendlyName = {});

  /**
   * Create a new client (script version).
   * @param friendlyName Friendly name for the client.
   * @note This is not thread safe! This is here to be called by scripts ONLY.
   * @return ID if the client created or -1 on error.
   */
  libobjgen::UUID CreateClientScript(const libcomp::String &friendlyName);

  /**
   * Delete a client.
   * @param pClient Handle to the client to delete.
   * @note This is not thread safe! This is here to be called by scripts ONLY.
   */
  void DeleteClient(logic::LogicWorker *pClient);

  /**
   * Delete a client.
   * @param client UUID of the client to delete.
   * @note This is not thread safe! This is here to be called by scripts ONLY.
   */
  bool DeleteClientScript(const libobjgen::UUID &uuid);

  /**
   * Wait for a script function to evaluate as true.
   * @param func Script function to evaluate.
   * @param timeout Time in seconds until timeout.
   * @return true if function evaluated to true; false on timeout or error.
   */
  bool WaitFor(Sqrat::Function func, double timeout = 1.0);

  /**
   * Wait for a message to be received by the worker.
   * @param messageType Message type to wait for.
   * @param func Script function to execute after the message is received.
   * @param timeout Time in seconds until timeout.
   * @return true if function evaluated to true; false on timeout or error.
   */
  bool WaitForMessage(int messageType, Sqrat::Function func,
                      double timeout = 1.0);

  /**
   * Wait for a client message to be received by the worker.
   * @param uuid UUID of the client to receive the message from (null UUID for
   * any client).
   * @param clientMessageType Client Message type to wait for.
   * @param func Script function to execute after the message is received.
   * @param timeout Time in seconds until timeout.
   * @return true if function evaluated to true; false on timeout or error.
   */
  bool WaitForClientMessage(const libobjgen::UUID &uuid, int clientMessageType,
                            Sqrat::Function func, double timeout = 1.0);

  /**
   * Run a script file and get the result.
   * @param path Path to the script file to run.
   * @return true if the script had no errors; false otherwise.
   */
  bool RunScript(const libcomp::String &path);

  /**
   * Register a callback to be called when a specific message type is received.
   * @param messageType Message type to call the callback for.
   * @param func Callback to call when the message type is received.
   */
  void RegisterMessageCallback(int messageType, Sqrat::Function func);

  /**
   * Register a callback to be called when a specific client message type is
   * received.
   * @param messageType Message type to call the callback for.
   * @param func Callback to call when the client message type is received.
   */
  void RegisterClientMessageCallback(int messageType, Sqrat::Function func);

  /**
   * Include another script file. To be called from a script.
   * @param path Path to the script file to include.
   * @returns true if the file parsed/executed OK; false otherwise.
   */
  bool ScriptInclude(const libcomp::String &path);

  /**
   * Set if the UI is active (to be read by scripts).
   * @param active If the UI is active.
   */
  void SetUIActive(bool active);

  /**
   * Check if the UI is active (to be read by scripts).
   * @returns true if the UI is active; false otherwise.
   */
  bool IsUIActive() const;

 private:
  /// If the UI is active.
  bool mUIActive;

  /// Script engine being used.
  libhack::ScriptEngine *mEngine;

  /// List of clients managed by this script worker.
  std::unordered_map<libobjgen::UUID, std::shared_ptr<logic::LogicWorker>>
      mClients;

  /// Callback when a client is created.
  std::function<void(ScriptWorker *, logic::LogicWorker *)> mCreateCallback;

  /// Callback when a client is deleted.
  std::function<void(ScriptWorker *, logic::LogicWorker *)> mDeleteCallback;

  /// Callback when a script is run.
  std::function<void(ScriptWorker *, const libcomp::String &, bool)>
      mScriptCallback;

  /// Script callbacks for messages.
  libcomp::EnumMultiMap<libcomp::Message::MessageType, Sqrat::Function>
      mMessageCallbacks;

  /// Script callbacks for messages.
  libcomp::EnumMultiMap<libcomp::Message::MessageClientType, Sqrat::Function>
      mClientMessageCallbacks;

  /// List of script files that have been loaded.
  std::set<libcomp::String> mIncludes;
};

}  // namespace script

#endif  // LIBCLIENT_SRC_SCRIPTWORKER_H
