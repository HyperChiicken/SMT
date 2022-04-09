/**
 * @file server/channel/src/packets/game/MitamaReunion.cpp
 * @ingroup channel
 *
 * @author HACKfrost
 *
 * @brief Request from the client to perform a mitama reunion reinforcement.
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
#include <DefinitionManager.h>
#include <ManagerPacket.h>
#include <Packet.h>
#include <PacketCodes.h>
#include <Randomizer.h>
#include <ServerConstants.h>

// Standard C++11 Includes
#include <math.h>

// object Includes
#include <DemonBox.h>
#include <MiDevilData.h>
#include <MiDevilLVUpRateData.h>
#include <MiGrowthData.h>
#include <MiMitamaReunionBonusData.h>
#include <MiUnionData.h>

// channel Includes
#include "ChannelServer.h"
#include "CharacterManager.h"
#include "FusionManager.h"
#include "TokuseiManager.h"

using namespace channel;

void HandleMitamaReunion(const std::shared_ptr<ChannelServer> server,
                         const std::shared_ptr<ChannelClientConnection> client,
                         int64_t mitamaID, int8_t reunionIdx) {
  auto characterManager = server->GetCharacterManager();
  auto definitionManager = server->GetDefinitionManager();
  auto fusionManager = server->GetFusionManager();

  auto state = client->GetClientState();
  auto cState = state->GetCharacterState();
  auto dState = state->GetDemonState();
  auto demon = dState->GetEntity();
  auto demonData = dState->GetDevilData();

  auto mitama = mitamaID ? std::dynamic_pointer_cast<objects::Demon>(
                               libcomp::PersistentObject::GetObjectByUUID(
                                   state->GetObjectUUID(mitamaID)))
                         : nullptr;
  auto mitamaData =
      mitama ? definitionManager->GetDevilData(mitama->GetType()) : nullptr;

  bool isMitama = characterManager->IsMitamaDemon(demonData);

  bool success = demon && mitama && demon != mitama && isMitama && mitamaData &&
                 reunionIdx >= 0 && reunionIdx < 12;
  size_t mitamaIdx =
      success ? fusionManager->GetMitamaIndex(
                    mitamaData->GetUnionData()->GetBaseDemonID(), success)
              : 0;

  if (success) {
    auto dbChanges = libcomp::DatabaseChangeSet::Create(state->GetAccountUID());

    success = characterManager->DoMitamaReunion(cState, (uint8_t)mitamaIdx,
                                                reunionIdx, dbChanges, false);

    if (success) {
      // Delete the mitama
      int8_t slot = mitama->GetBoxSlot();
      auto box = std::dynamic_pointer_cast<objects::DemonBox>(
          libcomp::PersistentObject::GetObjectByUUID(mitama->GetDemonBox()));

      characterManager->DeleteDemon(mitama, dbChanges);
      if (box) {
        characterManager->SendDemonBoxData(client, box->GetBoxID(), {slot});
      }

      server->GetWorldDatabase()->QueueChangeSet(dbChanges);
    }
  }

  if (!success) {
    // Send a packet indicating failure to the client. Success would have the
    // packet sent in DoMitamaReunion.
    libcomp::Packet reply;
    reply.WritePacketCode(ChannelToClientPacketCode_t::PACKET_MITAMA_REUNION);
    reply.WriteS8(success ? 0 : -1);
    reply.WriteS8(reunionIdx);
    reply.WriteS8((int8_t)0);   // Normally newIdx
    reply.WriteU8((uint8_t)0);  // Normally bonusID

    client->SendPacket(reply);
  }
}

bool Parsers::MitamaReunion::Parse(
    libcomp::ManagerPacket* pPacketManager,
    const std::shared_ptr<libcomp::TcpConnection>& connection,
    libcomp::ReadOnlyPacket& p) const {
  if (p.Size() != 9) {
    return false;
  }

  int64_t mitamaID = p.ReadS64Little();
  int8_t reunionIdx = p.ReadS8();

  auto server =
      std::dynamic_pointer_cast<ChannelServer>(pPacketManager->GetServer());
  auto client = std::dynamic_pointer_cast<ChannelClientConnection>(connection);

  server->QueueWork(HandleMitamaReunion, server, client, mitamaID, reunionIdx);

  return true;
}
