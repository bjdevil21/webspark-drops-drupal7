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
  public function prepareRollback($entity_id) {
    // Code to execute before an entity has been rolled back
    // Need to get new, mapped uid
    foreach ($entity_id as $new_uid) {
      db_delete('users_roles')
        ->condition('uid', $new_uid)
        ->condition('rid', 5)
        ->execute();
    }
  }

  public function bulkRollback(array $uids) {
    migrate_instrument_start('user_delete_multiple');
    $this->prepareRollback($uids);
    user_delete_multiple($uids);
    $this->completeRollback($uids);
    migrate_instrument_stop('user_delete_multiple');
  }

}