<?php

/**
 * Implements hook_views_data().
 */
function asu_comrf_views_data() {

  $data = array();
  $data['asu_comrf']['table']['group'] = t('ASU Change of Major Request Form');
  $data['exposed']['table']['base'] = array(
    'title' => t('ASU Change of Major Request Form'),
    'help' => t('Contains submissions records for the ASU Change of Major Request Form.'),
  );

  // sid
  $data['asu_comrf']['sid'] = array(
    'title' => t('sid'),
    'help' => t('The submission ID.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  // asurite_id
  $data['exposed']['asurite_id'] = array(
    'title' => t('ASURITE ID'),
    'help' => t('The ASURITE ID.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // new_major_choice
  $data['exposed']['new_major_choice'] = array(
    'title' => t('New major choice'),
    'help' => t('The new major choice.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // new_campus_choice
  $data['exposed']['new_campus_choice'] = array(
    'title' => t('New campus choice'),
    'help' => t('The new campus choice.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // com_requirements_text
  $data['exposed']['com_requirements_text'] = array(
    'title' => t('Requirements'),
    'help' => t('The change of major requirements for the new major choice.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // requirements_check
  $data['exposed']['requirements_check'] = array(
    'title' => t('Requirements check'),
    'help' => t('The requirements check for the new major choice.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // double_check
  $data['exposed']['double_check'] = array(
    'title' => t('Requirements double check'),
    'help' => t('The requirements double check for the new major choice.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // contact_phone_number
  $data['exposed']['contact_phone_number'] = array(
    'title' => t('Contact phone number'),
    'help' => t('The contact phone number.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // major_change_source
  $data['exposed']['major_change_source'] = array(
    'title' => t('Major change source'),
    'help' => t('The source for the change of major.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // major_change_source_other
  $data['exposed']['major_change_source_other'] = array(
    'title' => t('Major change source (other)'),
    'help' => t('The explanation of the source for the change of major.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // why_change
  $data['exposed']['why_change'] = array(
    'title' => t('Why change'),
    'help' => t('The reason why the major is being changed.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // why_change_other
  $data['exposed']['why_change_other'] = array(
    'title' => t('Why change (other)'),
    'help' => t('The explanation of the why the major is being changed.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // timestamp
  $data['asu_comrf']['timestamp'] = array(
    'title' => t('Timestamp'),
    'help' => t('The submission timestamp.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  return $data;

}