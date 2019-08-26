<?php

/**
 * Implements hook_views_data().
 */
function asu_comrf_views_data() {

  $data = array();
  $data['asu_comrf_form_submission']['table']['group'] = t('ASU Change of Major Request Form');
  $data['asu_comrf_form_submission']['table']['base'] = array(
    'title' => t('ASU Change of Major Request Form'),
    'help' => t('Contains submissions records for the ASU Change of Major Request Form.'),
  );

  // sid
  $data['asu_comrf_form_submission']['sid'] = array(
    'title' => t('sid'),
    'help' => t('The submission ID.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  // asurite_id
  $data['asu_comrf_form_submission']['asurite_id'] = array(
    'title' => t('ASURITE ID'),
    'help' => t('The ASURITE ID.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // major_to_change
  $data['asu_comrf_form_submission']['major_to_change'] = array(
    'title' => t('Major to change'),
    'help' => t('The major to change.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // new_major_choice
  $data['asu_comrf_form_submission']['new_major_choice'] = array(
    'title' => t('New major choice'),
    'help' => t('The new major choice.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // new_major_choice
  $data['asu_comrf_form_submission']['new_subplan_choice'] = array(
    'title' => t('New subplan choice (optional)'),
    'help' => t('A new subplan choice is required only in specific cases.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // new_campus_choice
  $data['asu_comrf_form_submission']['new_campus_choice'] = array(
    'title' => t('New campus choice'),
    'help' => t('The new campus choice.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // com_requirements_text
  $data['asu_comrf_form_submission']['com_requirements_text'] = array(
    'title' => t('Requirements'),
    'help' => t('The change of major requirements for the new major choice.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // requirements_check
  $data['asu_comrf_form_submission']['requirements_check'] = array(
    'title' => t('Requirements check'),
    'help' => t('The requirements check for the new major choice.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // double_check
  $data['asu_comrf_form_submission']['double_check'] = array(
    'title' => t('Requirements double check'),
    'help' => t('The requirements double check for the new major choice.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // contact_phone_number
  $data['asu_comrf_form_submission']['contact_phone_number'] = array(
    'title' => t('Contact phone number'),
    'help' => t('The contact phone number.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // major_change_source
  $data['asu_comrf_form_submission']['major_change_source'] = array(
    'title' => t('Major change source'),
    'help' => t('The source for the change of major.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // major_change_source_other
  $data['asu_comrf_form_submission']['major_change_source_other'] = array(
    'title' => t('Major change source (other)'),
    'help' => t('The explanation of the source for the change of major.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // why_change
  $data['asu_comrf_form_submission']['why_change'] = array(
    'title' => t('Why change'),
    'help' => t('The reason why the major is being changed.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // why_change_other
  $data['asu_comrf_form_submission']['why_change_other'] = array(
    'title' => t('Why change (other)'),
    'help' => t('The explanation of the why the major is being changed.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // timestamp
  $data['asu_comrf_form_submission']['timestamp'] = array(
    'title' => t('Timestamp'),
    'help' => t('The submission timestamp.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  return $data;

}