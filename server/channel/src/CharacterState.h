/**
 * @file server/channel/src/CharacterState.h
 * @ingroup channel
 *
 * @author HACKfrost
 *
 * @brief Represents the state of a player character on the channel.
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

#ifndef SERVER_CHANNEL_SRC_CHARACTERSTATE_H
#define SERVER_CHANNEL_SRC_CHARACTERSTATE_H

// objects Includes
#include <Character.h>

// channel Includes
#include <ActiveEntityState.h>

namespace libcomp {
class DefinitionManager;
}

namespace objects {
class DigitalizeState;
class EventCounter;
class Item;
class MiGuardianAssistData;
class MiSpecialConditionData;
}  // namespace objects

namespace channel {

/**
 * Contains the state of a player character on the channel.
 */
class CharacterState : public ActiveEntityStateImp<objects::Character> {
 public:
  /**
   * Create a new character state.
   */
  CharacterState();

  /**
   * Clean up the character state.
   */
  virtual ~CharacterState() {}

  /**
   * Get the tokusei effects IDs from the character's current equipment
   * @return Tokusei effect IDs from the character's current equipment
   */
  std::list<int32_t> GetEquipmentTokuseiIDs() const;

  /**
   * Get the tokusei effects IDs that are applied to the character from the
   * character's number of completed entries in the Demonic Compendium
   * @return Tokusei effect IDs that are applied to the character from the
   * character's number of completed entries in the Demonic Compendium
   */
  std::list<int32_t> GetCompendiumTokuseiIDs() const;

  /**
   * Get the conditional tokusei effect definitions from the character's
   * current equipment
   * @return Conditional tokusei effect definitions from the character's
   *  current equipment
   */
  std::list<std::shared_ptr<objects::MiSpecialConditionData>>
  GetConditionalTokusei() const;

  /**
   * Get the current number of complete quests that grant bonuses
   * @return Bonus granting complete quest count
   */
  uint32_t GetQuestBonusCount() const;

  /**
   * Get the current max fusion gauge stock count
   * @return Max fusion gauge stock count
   */
  uint8_t GetMaxFusionGaugeStocks() const;

  /**
   * Get the set of tokusei effect IDs granted by quest completion
   * @return List of tokusei effect IDs
   */
  std::list<int32_t> GetQuestBonusTokuseiIDs() const;

  /**
   * Get the current digitalization state of the character. This state
   * is calculated when digitalization starts so anything that affects
   * the calculations that occur at that time will not reflect until
   * digitalzation occurs again.
   * @return Pointer to the current digitalzation state of the character
   */
  std::shared_ptr<objects::DigitalizeState> GetDigitalizeState() const;

  /**
   * Begin digitalization between the character and the supplied demon
   * @param demon Pointer to the demon to digitalize with
   * @param definitionManager Pointer to the definition manager to use
   *  for determining digitalization information
   * @return Pointer to the new digitalization state, null if digitalization
   *  was ended
   */
  std::shared_ptr<objects::DigitalizeState> Digitalize(
      const std::shared_ptr<objects::Demon>& demon,
      libhack::DefinitionManager* definitionManager);

  /**
   * Get the current valuable based ability level of the character to
   * use digitalization from 0 (cannot use) to 2 (can use all types)
   * @return Numeric digitalization ability level
   */
  uint8_t GetDigitalizeAbilityLevel();

  /**
   * Gather equipment stats associated to the character
   * @param definitionManager Pointer to the definition manager to use
   *  for gathering item definitions
   * @param adjustments Output list of non-NRA adjustments
   * @param nraAdjustments Output list of NRA adjustments which have
   *  a unique format from every other source
   * @return true if the stats were retrieved, false if they were not
   */
  bool GetEquipmentStats(
      libhack::DefinitionManager* definitionManager,
      std::list<std::shared_ptr<objects::MiCorrectTbl>>& adjustments,
      std::list<std::shared_ptr<objects::MiCorrectTbl>>& nraAdjustments);

  /**
   * Determine the tokusei effects gained for the character based upon
   * their current equipment
   * @param definitionManager Pointer to the definition manager to use
   *  for determining equipment effects
   */
  void RecalcEquipState(libhack::DefinitionManager* definitionManager);

  /**
   * Determine if any equipment on the character is set to expire but has
   * not yet since the last time it was checked. If this returns true,
   * RecalcEquipState should be called again.
   * @param now Current system time
   * @return true if equipment has expired, false if it has not
   */
  bool EquipmentExpired(uint32_t now);

  /**
   * Update the IDs of tokusei that apply to the character due to the number of
   * entries completed in the Demonic Compendium
   * @param tokuseiIDs List of tokusei
   */
  void UpdateCompendiumTokuseiIDs(std::list<int32_t> tokuseiIDs);

  /**
   * Determine the quest bonus effects gained for the character based
   * on the number of completed quests. If a quest is being completed
   * this function should be used with the optional secondary parameter
   * supplied.
   * @param definitionManager Pointer to the definition manager to use for
   *  determining quest bonus effects
   * @param completedQuestID Optional ID of a quest that should be completed
   *  before recalculating. If not supplied, the entire quest completion
   *  list should be used to recalculate
   * @return true if the recalculation resulted in more quest bonuses being
   *  applied or increased
   */
  bool UpdateQuestState(libhack::DefinitionManager* definitionManager,
                        uint32_t completedQuestID = 0);

  /**
   * Determine the character's current expertise points for the
   * specified ID. This includes chain expertise calculations.
   * @param expertiseID ID of the expertise to calculate
   * @param definitionManager Pointer to the definition manager to use
   *  for determining chain expertise relationships. If null, the expertise
   *  will be retrieved as a non-chain expertise.
   * @return Total points of the supplied expertise ID
   */
  int32_t GetExpertisePoints(
      uint32_t expertiseID,
      libhack::DefinitionManager* definitionManager = nullptr);

  /**
   * Determine the character's current expertise rank for the
   * specified ID. This includes chain expertise calculations.
   * @param expertiseID ID of the expertise to calculate
   * @param definitionManager Pointer to the definition manager to use
   *  for determining chain expertise relationships. If null, the expertise
   *  will be retrieved as a non-chain expertise.
   * @return Rank of the supplied expertise ID
   */
  uint8_t GetExpertiseRank(
      uint32_t expertiseID,
      libhack::DefinitionManager* definitionManager = nullptr);

  /**
   * Determine if the character (or account) has a specific action cooldown
   * active.
   * @param cooldownID ID of the cooldown to check
   * @param accountLevel If true, the AccountWorldData level values are
   *  checked. If false, the character level values are checked.
   * @param refresh Refresh the cooldown values based on the current server
   *  time before checking, defaults to true
   * @return true if the cooldown is active, false if it is not
   */
  bool ActionCooldownActive(int32_t cooldownID, bool accountLevel,
                            bool refresh = true);

  /**
   * Get the event counter assigned to the character with a specified type
   * @param type Event type to retrieve
   * @param createIfMissing Create a new pointer to an event counter if
   *  one does not already exist and associate it to the character's state.
   *  The newly created counter does not save automatically since the caller
   *  may need to set non-default values.
   * @return Pointer to the event counter associated to the type
   */
  std::shared_ptr<objects::EventCounter> GetEventCounter(
      int32_t type, bool createIfMissing = false);

  /**
   * Refresh the action cooldowns for the character or associated account.
   * @param accountLevel If true, the AccountWorldData level values are
   *  refreshed. If false, the character level values are refreshed.
   * @param time System time representing the cut-off point for no longer
   *  active cooldowns. Any times before this value will have their
   *  cooldown removed.
   */
  void RefreshActionCooldowns(bool accountLevel, uint32_t time = 0);

  /**
   * Retrieve the reunion point count from the character's associated
   * AccountWorldData.
   * @param mitama If true, mitama points are retrieved. If false, non-mitama
   *  reunion points are retrieved.
   * @return Reunion point count associated to the character
   */
  uint32_t GetReunionPoints(bool mitama);

  /**
   * Recalculate the set of skills available to the character that are currently
   * disabled.
   * @param definitionManager Pointer to the DefinitionManager to use when
   *  determining skill definitions
   * @return the set of newly disabled skills
   */
  std::set<uint32_t> RecalcDisabledSkills(
      libhack::DefinitionManager* definitionManager);

  virtual const libobjgen::UUID GetEntityUUID();

  virtual uint8_t RecalculateStats(
      libhack::DefinitionManager* definitionManager,
      std::shared_ptr<objects::CalculatedEntityState> calcState = nullptr,
      std::shared_ptr<objects::MiSkillData> contextSkill = nullptr);

  virtual bool CopyToEnemy(const std::shared_ptr<ActiveEntityState>& eState,
                           libhack::DefinitionManager* definitionManager);

  virtual std::set<uint32_t> GetAllSkills(
      libhack::DefinitionManager* definitionManager, bool includeTokusei);

  virtual uint8_t GetLNCType();

  virtual int8_t GetGender();

  /**
   * Cast an EntityStateObject into a CharacterState. Useful for script
   * bindings.
   * @return Pointer to the casted CharacterState
   */
  static std::shared_ptr<CharacterState> Cast(
      const std::shared_ptr<EntityStateObject>& obj);

 private:
  /**
   * Calculate and update item fuse bonuses for the supplied equipment
   * @param definitionManager Pointer to the definition manager to use
   *  for determining equipment info
   * @param equipment Pointer to the equipment to calculate bonuses for
   */
  void AdjustFuseBonus(libhack::DefinitionManager* definitionManager,
                       std::shared_ptr<objects::Item> equipment);

  /// Tokusei effect IDs available due to the character's current
  /// equipment. Sources contain mod slots, equipment sets and
  /// enchantments. Can contain duplicates.
  std::list<int32_t> mEquipmentTokuseiIDs;

  /// Tokusei effect IDs that affect the character that are available due to
  /// number of entries in the character's Demonic Compendium
  std::list<int32_t> mCompendiumTokuseiIDs;

  /// List of tokusei conditions that apply based upon the state
  /// of the character other than base stats
  std::list<std::shared_ptr<objects::MiSpecialConditionData>>
      mConditionalTokusei;

  /// Tokusei effect IDs available due to the number of quests completed
  std::list<int32_t> mQuestBonusTokuseiIDs;

  /// Pointer to the current digitalization state of the character
  std::shared_ptr<objects::DigitalizeState> mDigitalizeState;

  /// System time for the next equipped item expiration to be checked
  /// at set intervals
  uint32_t mNextEquipmentExpiration;

  /// Quick access count representing the number of completed quests
  /// that can affect bonuses
  uint32_t mQuestBonusCount;

  /// The number of fusion gauge stocks the character has access to from
  /// equipment and valuables.
  uint8_t mMaxFusionGaugeStocks;

  /// Precalculated equipment fuse bonuses that are applied after base
  /// stats have been calculated (since they are all numeric adjustments)
  libcomp::EnumMap<CorrectTbl, int16_t> mEquipFuseBonuses;
};

}  // namespace channel

#endif  // SERVER_CHANNEL_SRC_CHARACTERSTATE_H
