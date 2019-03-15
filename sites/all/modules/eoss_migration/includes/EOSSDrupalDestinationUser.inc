<?php
/**
 * We add support for UUIDs - just in case they're needed.
 * Migrate d2d extension
 */

class EOSSDrupalDestinationUser extends MigrateDestinationUser {

  public function __construct(array $arguments) {
    parent::__construct($arguments);
  }

  /**
   * @param null $migration
   * @return array
   */
  /**
   * Implementation of Migration::fields().
   *
   * Bring in Path alias addition from DrupalDestinationUser and skip it
   *
   * @param $migration
   * @return $fields
   */
  public function fields($migration = NULL) {
    $fields = parent::fields($migration);
    $fields['path'] = t('Path alias');
    $fields['uuid'] = t('UUID');
    return $fields;
  }
}