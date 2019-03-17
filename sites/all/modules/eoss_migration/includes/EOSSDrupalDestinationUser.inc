<?php
/**
* We add support for UUIDs - just in case they're needed.
*/

class EOSSDrupalDestinationUser extends DrupalDestinationUser {
  /**
   * @param null $migration
   * @return array
   */
  public function fields($migration = NULL) {
    $fields = parent::fields($migration);
    $fields['uuid'] = t('UUID');
    unset($fields['field_first_name:language']);
    unset($fields['field_last_name:language']);
    return $fields;
  }

  /**
   * Rolls back role IDs for imported users (roles were not brought over in the user import).
   * @param $entity_id
   */
  public function prepareRollback($entity_id) {
    // Code to execute before an entity has been rolled back
    foreach ($entity_id as $new_uid) {
      db_delete('users_roles')
        ->condition('uid', $new_uid)
        ->condition('rid', 5)
        ->execute();
    }
  }

  /**
   * Need to declare this here to allow prepareRollback to work properly above
   * @param array $uids
   */
  public function bulkRollback(array $uids) {
    migrate_instrument_start('user_delete_multiple');
    $this->prepareRollback($uids);
    user_delete_multiple($uids);
    $this->completeRollback($uids);
    migrate_instrument_stop('user_delete_multiple');
  }

}