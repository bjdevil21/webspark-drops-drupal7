<?php
/**
 * @file
 * webspark_callout.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function websoark_callout_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'create fieldable callout'.
  $permissions['create fieldable callout'] = array(
    'name' => 'create fieldable callout',
    'roles' => array(
      'administrator' => 'administrator',
      'editor' => 'editor',
    ),
    'module' => 'fieldable_panels_panes',
  );

  // Exported permission: 'delete fieldable callout'.
  $permissions['delete fieldable callout'] = array(
    'name' => 'delete fieldable callout',
    'roles' => array(
      'administrator' => 'administrator',
      'editor' => 'editor',
    ),
    'module' => 'fieldable_panels_panes',
  );

  // Exported permission: 'edit fieldable callout'.
  $permissions['edit fieldable callout'] = array(
    'name' => 'edit fieldable callout',
    'roles' => array(
      'administrator' => 'administrator',
      'editor' => 'editor',
    ),
    'module' => 'fieldable_panels_panes',
  );

  return $permissions;
}
