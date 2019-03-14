<?php
/**
* We add support for UUIDs - just in case they're needed.
*/

class EOSSDrupalDestinationUser extends DrupalDestinationUser {
  /**
   * Implementation of Migration::fields().
   *
   * @param $migration
   * @return $fields
   */
  public function fields($migration = NULL) {
    $fields = parent::fields($migration);
    $fields['uuid'] = t('UUID');
    return $fields;
  }
}