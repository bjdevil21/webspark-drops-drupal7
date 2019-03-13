<?php
/**
 * @file
 * Implementation of DrupalUser7Migration for EOSS.
 */

class EOSSSubmissionUsers extends DrupalUser7Migration {

  public function __construct(array $arguments) {
    parent::__construct($arguments);
    // Add UUID field
    $this->sourceFields['uuid'] = t('UUID');
      // @HOWTO Field Mappings:
      // Link the source to the destination field.
      // Can do basic processing (|)
      // Can hold arguments to pass onto the destination field
    $this->addFieldMapping('uuid', 'uuid');
    $this->destination = new EOSSDestinationUser();
  }

  /**
   * Query for the basic user data, butpare down the user list to return only
   * users that have an entry in the webform_submissions table.
   *
   * @return QueryConditionInterface
   */
  protected function query() {
    /* QUERY: SELECT u.* FROM {users} u
    INNER JOIN {webform_submissions} ws ON u.uid = ws.uid
    WHERE  (u.uid > '0')
    GROUP BY u.uid
    ORDER BY u.uid ASC */
    // below same as db_select
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('users', 'u');
    $query->innerJoin('webform_submissions', 'ws', 'u.uid = ws.uid');
    $query->fields('u')
      ->condition('u.uid', 0, '>')
      ->groupBy('u.uid')
      ->orderBy('u.uid', 'ASC');
    return $query;
  }

  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    // Check for duplicate user names in destination
    // Don't insert or update if exists already
    /*
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('users', 'u')
      ->fields('u', array('name'))
      ->condition('u.name', $row->name)
      ->execute()->fetchAll();
    if (count($query) > 0) {
      return FALSE;
    }*/
  }

  /**
   * Implementation of Migration::complete().
   * Insert CAS role (ID: 5) row into users_roles when user has been added
   *
   * @param $account
   * @param $row
   */
  public function complete($account, $row) {
    parent::complete($account, $row);

    $query = db_insert('users_roles', 'ur') // Table name no longer needs {}
    ->fields('ur', array(
      'uid' => $row->uid,
      'rid' => 5,
    ));
    $query->execute();
  }
}

// @HOWTO Migration maps -> Allows you to look up the old, source IDs based on
// on the destination IDs.

// @HOWTO -> To do a basic migration, you only need to set the following
// in __construct:
  // $this->source
    // Pass in a Database API-written $query into the source class of choice
    // (usually MigrateSourceSQL($query))
  // $this->destination
    // Pass in a new class instantiation - new MigrateDestinationNode('<contentType'), etc.
    // Not all classes here need arguments... Look at the class in question for more info.
  // $this->map
    // Used for passing in schema of your keyed rows (usually IDs - uid, nid, etc.)
    // Ex. $this->map = new MigrateSQLMap(
      // $this->machineName, // set this in arguments
      // array(), // array with the SOURCE DB schema (see hook_schema) for your IDs
      // array(), // Contains the DESTINATION DB schema (see hook schema)
        // There's usually a helper function provided by destination code
        // Ex. MigrateDestinationTerm::getKeySchema()
    // );
  // $this->addFieldMapping('dest', 'src');
    // Maps an individual src field to the dest. field
    // This can take ->arguments($arguments)... This is used when multiple source fields
    // or values need to be passed into a single destination field. (Ex. body->summary,
    // body->format, etc. - all part of node->body.
    // Workaround: pass an array into arguments() and use key => value pairs to pass in that data.
    // See https://www.drupal.org/node/1012810 for more possible workarounds for this.
    // https://youtu.be/3fG4wBvLrOI?t=1257

// These four values coincide with the four top level Migrate classes (alongside Migration [base])
  // MigrateSource
  // MigrateDestination
  // MigrateMap
  // MigrateFieldMapping

// @HOWTO -> Dependencies -- When you set a dependency, you have to run that migration BEFORE
// the current migration can be run. (Set this in $arguments.)
  // Ex. You have the old IDs from the source and you want to get new IDs for users
  // in the destination:
  // $this->addFieldMapping('uid', 'old_id')->sourceMigration('<previousMigration>');
  // $this->dependencies = array('<previousMigration>');

// @HOWTO - Circular dependencies (need the user profile that doesn't exist yet for that billing
// table, and you need the billing info for the user profile).
// SOLUTION: Add a sourceMigration('<previousMigration') to your <newMigration> class
// (like above), then add createStub($newMigration, $source_key) method call to your
// <previousMigration> class.
// This creates a new but empty record that gives you that new ID. When <previousMigration>
// runs, it'll go to the $migration (the "2nd" migration) and create the record stub.
// Then when the 2nd migration runs, it updates and populate the stub record with the right info.
// https://youtu.be/3fG4wBvLrOI?t=1503

// @HOWTO - Additional, important methods in your Migrate class (after __construct in
// code; presented in execution order):
  // prepareRow($row) -- Add or change values by editing the $row object properties,
  // which contains your raw SQL data for that record (keyed by SQL col name by default)
    // Can skip this record by return FALSE based on any if/then PHP logic you want to use.
  // THEN
  // the field handlers modify the values into a more Drupal-friendly value (nested arrays, etc.)
  // THEN
  // prepare($entity, $row) -- Can edit the now Drupal-generated entity ($entity) before
  // Drupal finally saves it.
    // The $row object is available for reference, but editing it does nothing.
  // THEN the $entity is saved by the destination (handlers?)
  // THEN
  // complete($ent, $row) -- Post-import processing for doing other work RELATED TO
  // the newly-minted entity $ent, like adding roles to a new user, etc.
    // The new $entity ID is now available for viewing, if needed.
    // You shouldn't have to save anything related to the entity. If so, you're doing it wrong.
  // THEN
  // The migration map is updated with the old and new IDs (based on what schema is
  // defined in the map)
