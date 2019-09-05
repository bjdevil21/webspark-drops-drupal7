<?php

/**
 * Retrieves degree program data from Degree Search.
 *
 * @return array
 *
 */
function _asu_comrf_get_degree_list_data() {

  // Loading helper function
  module_load_include('inc','asu_comrf','includes/asu_comrf.program_data_helpers');

  // Get environment specific URLs
  $degree_search_env = variable_get('asu_comrf_degree_search_env');
  $url = 'https://' . variable_get('asu_comrf_degree_search_url_' . $degree_search_env);

  // Compose XML call
  $degree_search_service_method = variable_get('asu_comrf_degree_search_service_method', 'findAllDegreesWithLessFields');

  $xml_service_method = 'eAdvisorDSFind.' . $degree_search_service_method;
  $xml_next_term_code = _asu_comrf_get_next_term_code(); // Get next term code
  $xml_degree_set = 'undergrad'; // Only return undergraduate programs
  $xml_include_certs = 0; // Don't include minors or certs
  $xml_to_send = '<?xml version="1.0"?>'.
      '<methodCall>'.
        '<methodName>'.$xml_service_method.'</methodName>'.
          '<params>'.
            '<param>'.
              '<value><string>'.$xml_next_term_code.'</string></value>'.
            '</param>'.
            '<param>'.
              '<value><string>'.$xml_degree_set.'</string></value>'.
            '</param>'.
            '<param>'.
              '<value><boolean>'.$xml_include_certs.'</boolean></value>'.
            '</param>'.
          '</params>'.
      '</methodCall>';

  $options = array(
    'headers' => array(
      "Content-Type" => 'application/xml',
    ),
    'method' => 'POST',
    'data' => $xml_to_send,
    'timeout' => 60,
  );

  // Execute call to Degree Search and store results in a variable
  $result = drupal_http_request($url, $options);
  $data = $result->data;

  // Do conversion magic to get the returned program data into array form instead of XML
  $xml = simplexml_load_string($data);
  $xml_data = $xml->params->param->value->array->data;
  $json = json_encode($xml_data);
  $programs_list_obj = json_decode($json);
  $programs_list_obj_value = $programs_list_obj->value;

  // Define order/index of fields from Degree Search call (this can and will change)
  $acad_plan_result_index = variable_get('asu_comrf_degree_search_result_acad_plan_index', 0); // 0 = First returned field on Degree Search call
  $degree_name_result_index = variable_get('asu_comrf_degree_search_result_degree_index', 1);
  $change_major_result_index = variable_get('asu_comrf_degree_search_result_change_major_index', 3);
  $trnscr_descr_result_index = variable_get('asu_comrf_degree_search_result_trnscr_descr_index', 4);
  $diploma_descr_result_index = variable_get('asu_comrf_degree_search_result_diploma_descr_index', 5);
  $subplans_array_result_index = variable_get('asu_comrf_degree_search_result_subplans_array_result_index', 6);
  $campus_string_array_result_index = variable_get('asu_comrf_degree_search_result_campus_string_array_index', 7);

  // Initialize the array for the list of degree programs
  $list_of_degrees = array();

  // Parse the data for desired information
  foreach($programs_list_obj_value as $degree_key => $struct) {

    $degree_program = $programs_list_obj->value[$degree_key]->struct->member;
    $loop_run = 1;
    foreach($degree_program as $key => $value) {
      if ($loop_run === 1) { // This runs for every field (at least a dozen times).
        $acad_plan_name = $programs_list_obj->value[$degree_key]->struct->member[$acad_plan_result_index]->name;
        $acad_plan_value = $programs_list_obj->value[$degree_key]->struct->member[$acad_plan_result_index]->value;

        $degree_name = $programs_list_obj->value[$degree_key]->struct->member[$degree_name_result_index]->name;
        $degree_value = $programs_list_obj->value[$degree_key]->struct->member[$degree_name_result_index]->value;
        $trnscr_descr_name = $programs_list_obj->value[$degree_key]->struct->member[$trnscr_descr_result_index]->name;
        $trnscr_descr_value = $programs_list_obj->value[$degree_key]->struct->member[$trnscr_descr_result_index]->value;
        $diploma_descr_name = $programs_list_obj->value[$degree_key]->struct->member[$diploma_descr_result_index]->name;
        $diploma_descr_value = $programs_list_obj->value[$degree_key]->struct->member[$diploma_descr_result_index]->value;
        $change_major_name = $programs_list_obj->value[$degree_key]->struct->member[$change_major_result_index]->name;
        $change_major_value = $programs_list_obj->value[$degree_key]->struct->member[$change_major_result_index]->value;

        // Skip non-degree majors (JIRA CMP2-36)
        if ($change_major_value === 'NO-DEGREE') {
          break 1;
        }

        $campus_string_array_name = $programs_list_obj->value[$degree_key]->struct->member[$campus_string_array_result_index]->name;
        $campus_string_array_value = array();
        $campus_string_array_value_raw = $programs_list_obj->value[$degree_key]->struct->member[$campus_string_array_result_index]->value->array->data->value;

        // Check only if not empty
        $subplans_name = $programs_list_obj->value[$degree_key]->struct->member[$subplans_array_result_index]->name;
        $subplans_value = $programs_list_obj->value[$degree_key]->struct->member[$subplans_array_result_index]->value->struct;

        if (!($subplans_value == new stdClass())) { // has object properties
          $subplans_value = $programs_list_obj->value[$degree_key]->struct->member[$subplans_array_result_index]->value->struct->member;
        }

        // Campus array
        if (is_array($campus_string_array_value_raw)) {
          foreach ($campus_string_array_value_raw as $key2 => $value2) {
            $campus_string_array_value[$key2] = $value2;
          }
        }
        else {
          $campus_string_array_value[0] = $campus_string_array_value_raw;
        }
        // Combining values into single array
        $list_of_degrees[$degree_key][$acad_plan_name] = $acad_plan_value;
        $list_of_degrees[$degree_key][$degree_name] = $degree_value;
        $list_of_degrees[$degree_key][$trnscr_descr_name] = $trnscr_descr_value;
        $list_of_degrees[$degree_key][$diploma_descr_name] = $diploma_descr_value;
        $list_of_degrees[$degree_key][$change_major_name] = $change_major_value;
        $list_of_degrees[$degree_key][$campus_string_array_name] = $campus_string_array_value;
        $list_of_degrees[$degree_key][$subplans_name] = $subplans_value;

        $loop_run++; // only iterate once
      }
    }
  }

  // Sort the degree program list by the TrnscrDescr value
  usort($list_of_degrees, '_asu_comrf_sort_degree_list_by_trnscr_descr');

  return $list_of_degrees;
}

/**
 * Sorts the degree list by TrnscrDescr value (e.g. degree program nice name).
 *
 * @param $a
 * @param $b
 * @return int
 */
function _asu_comrf_sort_degree_list_by_trnscr_descr($a, $b) {
  return strcmp($a["TrnscrDescr"], $b["TrnscrDescr"]);
}

/**
 * Inserts an incoming list of degree programs from Degree Search into the database.
 *
 * @param $list_of_degrees
 * @return bool
 */
function _asu_comrf_store_degree_list($list_of_degrees) {
  // Initialize check variables
  $data_stored_successfully = FALSE;
  $stored_data = '';

  // Insert data from array to database
  if ($list_of_degrees) {
    $list_of_degrees_s = serialize($list_of_degrees);
    $stored_data = db_insert('asu_comrf_degree_list')
      ->fields(array(
        'degree_programs' => $list_of_degrees_s,
        'timestamp' => date('U'),
      ))
      ->execute();
  }
  // If inserting data was successful:
  // 1) set check variable to true
  // 2) Delete data sets older than 30 days if more than 10 records are in the DB.
  if ($stored_data !== NULL) {
    $data_stored_successfully = TRUE;
    // Count number of rows (list_of_majors entries)
    $results = db_select('asu_comrf_degree_list')->fields(NULL, array('timestamp'))->countQuery()->execute()->fetchField();
    if ($results > 10) {
      $cutoff_date = date('U', strtotime('-30 days'));
      db_delete('asu_comrf_degree_list')
        ->condition('timestamp', $cutoff_date, '<=')
        ->execute();
      watchdog('asu_comrf', 'Degree lists stored more than 30 days have been deleted.' . __FUNCTION__
        . '.', array(), WATCHDOG_NOTICE);
    }
  } else {
    watchdog('asu_comrf', 'The degree list could not be stored when cron last ran. Check out ' . __FUNCTION__
      . '.', array(), WATCHDOG_ERROR);
  }

  return $data_stored_successfully;
}
