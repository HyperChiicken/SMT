/**
 * @file server/channel/src/packets/game/TriFusionRewardUpdate.cpp
 * @ingroup channel
 *
 * @author HACKfrost
 *
 * @brief Request from the client to update tri-fusion success rewards.
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
#include <Log.h>
#include <ManagerPacket.h>
#include <Packet.h>
#include <PacketCodes.h>

// object Includes
#include <Item.h>
#include <ItemBox.h>
#include <MiItemBasicData.h>
#include <MiItemData.h>
#include <PlayerExchangeSession.h>
#include <TriFusionHostSession.h>

// channel Includes
#include "ChannelServer.h"
#include "CharacterManager.h"
#include "DefinitionManager.h"
#include "ManagerConnection.h"

using namespace channel;

bool Parsers::TriFusionRewardUpdate::Parse(
    libcomp::ManagerPacket* pPacketManager,
    const std::shared_ptr<libcomp::TcpConnection>& connection,
    libcomp::ReadOnlyPacket& p) const {
  if (p.Size() != 13) {
    return false;
  }

  int64_t itemID = p.ReadS64Little();
  int32_t participantID = p.ReadS32Little();
  int8_t slotID = p.ReadS8();

  auto server =
      std::dynamic_pointer_cast<ChannelServer>(pPacketManager->GetServer());
  auto characterManager = server->GetCharacterManager();
  auto managerConnection = server->GetManagerConnection();

  auto client = std::dynamic_pointer_cast<ChannelClientConnection>(connection);
  auto state = client->GetClientState();
  auto cState = state->GetCharacterState();
  auto inventory = cState->GetEntity()->GetItemBoxes(0).Get();
  auto exchangeSession = state->GetExchangeSession();
  auto tfSession =
      std::dynamic_pointer_cast<objects::TriFusionHostSession>(exchangeSession);

  auto item = std::dynamic_pointer_cast<objects::Item>(
      libcomp::PersistentObject::GetObjectByUUID(state->GetObjectUUID(itemID)));
  auto itemDef =
      item ? server->GetDefinitionManager()->GetItemData(item->GetType())
           : nullptr;

  std::set<int32_t> participantIDs;

  bool failure = exchangeSession == nullptr || item == nullptr;
  if (item &&
      (!itemDef || (itemDef->GetBasic()->GetFlags() & ITEM_FLAG_TRADE) == 0)) {
    LogTradeError([item, state]() {
      return libcomp::String(
                 "Player attempted to add non-trade item type %1 to a "
                 "trifusion reward: %2\n")
          .Arg(item->GetType())
          .Arg(state->GetAccountUID().ToString());
    });

    failure = true;
  } else if (!tfSession && exchangeSession) {
    // Guest cancelled
    auto otherCState = std::dynamic_pointer_cast<CharacterState>(
        exchangeSession->GetOtherCharacterState());
    auto otherClient = otherCState ? managerConnection->GetEntityClient(
                                         otherCState->GetEntityID(), false)
                                   : nullptr;
    auto otherState = otherClient ? otherClient->GetClientState() : nullptr;
    tfSession = otherState
                    ? std::dynamic_pointer_cast<objects::TriFusionHostSession>(
                          otherState->GetExchangeSession())
                    : nullptr;

    failure = tfSession == nullptr;
  } else if (tfSession) {
    for (auto pState : tfSession->GetGuests()) {
      participantIDs.insert(pState->GetEntityID());
    }

    if (participantIDs.find(participantID) == participantIDs.end()) {
      LogGeneralErrorMsg(
          "Invalid participant ID supplied for TriFusion reward update "
          "request\n");

      failure = true;
    } else if (slotID >= 4) {
      LogGeneralErrorMsg("Invalid TriFusion reward slot ID supplied\n");

      failure = true;
    } else {
      auto targetState =
          ClientState::GetEntityClientState(participantID, false);
      auto targetExchange =
          targetState ? targetState->GetExchangeSession() : nullptr;
      if (targetExchange) {
        bool found = false;
        if (item && item->GetItemBox() == inventory->GetUUID()) {
          // Detect if the item is somewhere else
          for (auto participant : participantIDs) {
            auto participantState =
                ClientState::GetEntityClientState(participant, false);
            auto participantExchange =
                participantState ? participantState->GetExchangeSession()
                                 : nullptr;

            if (participantExchange) {
              auto items = participantExchange->GetItems();
              for (size_t i = 0; i < 4; i++) {
                if (items[i].Get() == item) {
                  if (slotID < 0) {
                    // Remove the item.
                    participantExchange->SetItems(i, NULLUUID);
                  }
                  found = true;
                  break;
                }
              }
            }

            if (found) {
              break;
            }
          }

          if (slotID >= 0) {
            // Not allowed to have been found already.
            failure = found;

            if (!failure) {
              targetExchange->SetItems((size_t)slotID, item);
            }
          } else {
            // Send a failure if it was never found somewhere.
            failure = !found;
          }
        } else {
          LogTradeError([state]() {
            return libcomp::String(
                       "Player attempted to add a phantom triple fusion reward "
                       "item: %1\n")
                .Arg(state->GetAccountUID().ToString());
          });

          failure = true;
        }
      } else {
        LogGeneralErrorMsg(
            "TriFusion reward update target is not a participant\n");

        failure = true;
      }
    }
  }

  libcomp::Packet reply;
  reply.WritePacketCode(
      ChannelToClientPacketCode_t::PACKET_TRIFUSION_REWARD_UPDATE);
  reply.WriteS8(failure ? 1 : 0);

  if (!failure) {
    reply.WriteS64Little(itemID);
    reply.WriteS32Little(participantID);
    reply.WriteS8(slotID);
  }

  client->SendPacket(reply);

  if (!failure) {
    // Notify all participants of the change
    std::list<std::shared_ptr<ChannelClientConnection>> pClients;
    for (int32_t pID : participantIDs) {
      auto pClient = managerConnection->GetEntityClient(pID, false);
      if (pClient) {
        pClients.push_back(pClient);
      }
    }

    if (pClients.size() > 0) {
      libcomp::Packet notify;
      notify.WritePacketCode(
          ChannelToClientPacketCode_t::PACKET_TRIFUSION_REWARD_UPDATED);
      notify.WriteS32Little(participantID);
      notify.WriteS8(slotID);

      // Double back and write for client specifically
      notify.WriteS64Little(0);

      if (slotID >= 0) {
        characterManager->GetItemDetailPacketData(notify, item);
      }

      // Create a copy for each participant with a local item ID
      for (auto pClient : pClients) {
        auto pState = pClient->GetClientState();

        libcomp::Packet nCopy(notify);

        int64_t objID = pState->GetObjectID(item->GetUUID());
        if (objID <= 0) {
          objID = server->GetNextObjectID();
          pState->SetObjectID(item->GetUUID(), objID);
        }

        nCopy.Seek(7);
        nCopy.WriteS64Little(objID);

        pClient->SendPacket(nCopy);
      }
    }
  }

  return true;
}
