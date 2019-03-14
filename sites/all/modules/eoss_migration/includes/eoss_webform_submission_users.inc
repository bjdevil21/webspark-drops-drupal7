<?php
/**
 * @file
 * Implementation of DrupalUser7Migration for EOSS.
 */

class EOSSWebformSubmissionUsers extends DrupalUser7Migration {
  /**
   * EOSSSubmissionUsers constructor.
   * @param array $arguments
   */
  public function __construct(array $arguments) {
    parent::__construct($arguments);
    // Add UUID field (sourceFields defined initially in parent class)
    $this->sourceFields['uuid'] = t('UUID');
    $this->addFieldMapping('uuid', 'uuid');
    // Extends DrupalDestinationUser() DrupalUser7Migration()
    $this->destination = new EOSSDrupalDestinationUser();
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
