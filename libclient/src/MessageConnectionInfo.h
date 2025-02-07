/**
 * @file libcomp/src/MessageConnectionInfo.h
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Client message.
 *
 * This file is part of the COMP_hack Client Library (libclient).
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

#ifndef LIBCLIENT_SRC_MESSAGECONNECTIONINFO_H
#define LIBCLIENT_SRC_MESSAGECONNECTIONINFO_H

// libobjgen Includes
#include <UUID.h>

// libcomp Includes
#include <CString.h>
#include <MessageClient.h>

namespace logic {

/**
 * Message signifying that a connection should be made to a server.
 */
class MessageConnectionInfo : public libcomp::Message::MessageClient {
 public:
  /**
   * Create the message.
   * @param uuid Client UUID this message is involved with.
   * @param connectionID ID for the connection.
   * @param host Host to connect to.
   * @param port Port on the host to connect to.
   */
  MessageConnectionInfo(const libobjgen::UUID& uuid,
                        const libcomp::String& connectionID,
                        const libcomp::String& host, uint16_t port)
      : libcomp::Message::MessageClient(uuid),
        mHost(host),
        mPort(port),
        mConnectionID(connectionID) {}

  /**
   * Cleanup the message.
   */
  ~MessageConnectionInfo() override {}

  /**
   * Get the host to connect to.
   * @returns Host to connect to.
   */
  libcomp::String GetHost() const { return mHost; }

  /**
   * Get the port to connect to.
   * @returns Port on the host to connect to.
   */
  uint16_t GetPort() const { return mPort; }

  /**
   * Get the connection ID.
   * @returns Connection ID.
   */
  libcomp::String GetConnectionID() const { return mConnectionID; }

 protected:
  /// Host to connect to.
  libcomp::String mHost;

  /// Port on the host to connect to.
  uint16_t mPort;

  /// Connection ID.
  libcomp::String mConnectionID;
};

/**
 * Message signifying that a connection should be made to a lobby server.
 */
class MessageConnectToLobby : public MessageConnectionInfo {
 public:
  /**
   * Create the message.
   * @param uuid Client UUID this message is involved with.
   * @param username Username to authenticate with.
   * @param password Password to authenticate with.
   * @param clientVersion Version of the client to tell the lobby.
   * @param connectionID ID for the connection.
   * @param host Host to connect to.
   * @param port Port on the host to connect to.
   * @param machineUUID UUID of the machine the client is connecting from.
   */
  MessageConnectToLobby(const libobjgen::UUID& uuid,
                        const libcomp::String& username,
                        const libcomp::String& password,
                        uint32_t clientVersion = 1666,
                        const libcomp::String& connectionID = "lobby",
                        const libcomp::String& host = "127.0.0.1",
                        uint16_t port = 10666,
                        const libobjgen::UUID& machineUUID = NULLUUID)
      : MessageConnectionInfo(uuid, connectionID, host, port),
        mUsername(username),
        mPassword(password),
        mClientVersion(clientVersion),
        mMachineUUID(machineUUID) {}

  /**
   * Cleanup the message.
   */
  ~MessageConnectToLobby() override {}

  Message* Clone() const override { return new MessageConnectToLobby(*this); }

  /**
   * Get the username for authentication.
   * @returns Username for authentication.
   */
  libcomp::String GetUsername() const { return mUsername; }

  /**
   * Get the password for authentication.
   * @returns Password for authentication.
   */
  libcomp::String GetPassword() const { return mPassword; }

  /**
   * Get the client version for authentication.
   * @returns Client version for authentication.
   */
  uint32_t GetClientVersion() const { return mClientVersion; }

  /**
   * Get the UUID of the machine the client is connecting from.
   * @returns UUID of the machine the client is connecting from.
   */
  libobjgen::UUID GetMachineUUID() const { return mMachineUUID; }

  /**
   * Get the specific client message type.
   * @return The message's client message type
   */
  libcomp::Message::MessageClientType GetMessageClientType() const override {
    return libcomp::Message::MessageClientType::CONNECT_TO_LOBBY;
  }

  /**
   * Dump the message for logging.
   * @return String representation of the message.
   */
  libcomp::String Dump() const override {
    return libcomp::String(
               "Message: Connect to lobby server\n"
               "ID: %1\nServer: %2:%3\nUsername: %4\nPassword: %5")
        .Arg(mConnectionID)
        .Arg(mHost)
        .Arg(mPort)
        .Arg(mUsername)
        .Arg(mPassword);
  }

 private:
  /// Username for authentication.
  libcomp::String mUsername;

  /// Password for authentication.
  libcomp::String mPassword;

  /// Client version for authentication.
  uint32_t mClientVersion;

  /// UUID of the machine the client is connecting from.
  libobjgen::UUID mMachineUUID;
};

/**
 * Message signifying that a connection should be made to a channel server.
 */
class MessageConnectToChannel : public MessageConnectionInfo {
 public:
  /**
   * Create the message.
   * @param uuid Client UUID this message is involved with.
   * @param sesisonKey Session key passed from the lobby.
   * @param connectionID ID for the connection.
   * @param host Host to connect to.
   * @param port Port on the host to connect to.
   */
  MessageConnectToChannel(const libobjgen::UUID& uuid, uint32_t sessionKey,
                          const libcomp::String& connectionID = "channel",
                          const libcomp::String& host = "127.0.0.1",
                          uint16_t port = 14666)
      : MessageConnectionInfo(uuid, connectionID, host, port),
        mSessionKey(sessionKey) {}

  /**
   * Cleanup the message.
   */
  ~MessageConnectToChannel() override {}

  Message* Clone() const override { return new MessageConnectToChannel(*this); }

  /**
   * Get the session key passed from the lobby.
   * @returns Session key passed from the lobby.
   */
  uint32_t GetSessionKey() const { return mSessionKey; }

  /**
   * Get the specific client message type.
   * @return The message's client message type
   */
  libcomp::Message::MessageClientType GetMessageClientType() const override {
    return libcomp::Message::MessageClientType::CONNECT_TO_CHANNEL;
  }

  /**
   * Dump the message for logging.
   * @return String representation of the message.
   */
  libcomp::String Dump() const override {
    return libcomp::String(
               "Message: Connect to channel server\n"
               "ID: %1\nServer: %2:%3\nSession Key: %4")
        .Arg(mConnectionID)
        .Arg(mHost)
        .Arg(mPort)
        .Arg(mSessionKey);
  }

 private:
  /// Session key passed from the lobby.
  uint32_t mSessionKey;
};

/**
 * Message signifying that a connection should be closed.
 */
class MessageConnectionClose : public libcomp::Message::MessageClient {
 public:
  /**
   * Create the message.
   * @param uuid Client UUID this message is involved with.
   */
  MessageConnectionClose(const libobjgen::UUID& uuid)
      : libcomp::Message::MessageClient(uuid) {}

  /**
   * Cleanup the message.
   */
  ~MessageConnectionClose() override {}

  Message* Clone() const override { return new MessageConnectionClose(*this); }

  /**
   * Get the specific client message type.
   * @return The message's client message type
   */
  libcomp::Message::MessageClientType GetMessageClientType() const override {
    return libcomp::Message::MessageClientType::CONNECTION_CLOSE;
  }

  /**
   * Dump the message for logging.
   * @return String representation of the message.
   */
  libcomp::String Dump() const override { return "Message: Close connection"; }
};

}  // namespace logic

#endif  // LIBCLIENT_SRC_MESSAGECONNECTIONINFO_H
