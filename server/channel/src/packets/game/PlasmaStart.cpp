/**
 * @file server/channel/src/packets/game/PlasmaStart.cpp
 * @ingroup channel
 *
 * @author HACKfrost
 *
 * @brief Request from the client to start the plasma picking minigame.
 *
 * This file is part of the Channel Server (channel).
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

#include "Packets.h"

// libcomp Includes
#include <ManagerPacket.h>
#include <Packet.h>
#include <PacketCodes.h>

// libhack Includes
#include <Log.h>

// channel Includes
#include "ChannelServer.h"
#include "CharacterManager.h"
#include "EventManager.h"
#include "PlasmaState.h"

using namespace channel;

bool Parsers::PlasmaStart::Parse(
    libcomp::ManagerPacket* pPacketManager,
    const std::shared_ptr<libcomp::TcpConnection>& connection,
    libcomp::ReadOnlyPacket& p) const {
  if (p.Size() != 5) {
    return false;
  }

  int32_t plasmaID = p.ReadS32Little();
  int8_t pointID = p.ReadS8();

  auto client = std::dynamic_pointer_cast<ChannelClientConnection>(connection);
  auto state = client->GetClientState();
  auto zone = state->GetZone();

  auto server =
      std::dynamic_pointer_cast<ChannelServer>(pPacketManager->GetServer());
  auto eventManager = server->GetEventManager();

  auto pState =
      zone ? std::dynamic_pointer_cast<PlasmaState>(zone->GetEntity(plasmaID))
           : nullptr;

  bool success = false;
  if (pState && pointID && eventManager->StartSystemEvent(client, plasmaID)) {
    auto cState = state->GetCharacterState();
    auto point = pState->GetPoint((uint32_t)pointID);
    if (point && !cState->CanInteract(point)) {
      LogGeneralWarning([&]() {
        return libcomp::String(
                   "Player is either too far from plasma in zone %1 to "
                   "interact with it or does not have line of sight: %2\n")
            .Arg(zone->GetDefinitionID())
            .Arg(state->GetAccountUID().ToString());
      });
    } else {
      success = pState->PickPoint(point, state->GetWorldCID());
    }

    if (!success) {
      eventManager->HandleEvent(client, nullptr);
    }
  }

  libcomp::Packet reply;
  reply.WritePacketCode(ChannelToClientPacketCode_t::PACKET_PLASMA_START);
  reply.WriteS32Little(plasmaID);
  reply.WriteS8(pointID);
  reply.WriteS32Little(success ? 0 : -1);

  client->SendPacket(reply);

  return true;
}
