<?php

/**
 * Retrieves degree program data from Degree Search.
 *
 * @return array
 *
 */
function asu_comrf_get_degree_list_data() {

  // Loading helper function
  module_load_include('inc','asu_comrf','includes/asu_comrf.program_data_helpers');

  // Get environment specific URLs
  $asu_comrf_degree_search_env = variable_get('asu_comrf_degree_search_env');
  if ($asu_comrf_degree_search_env == 'dev') {
    $url = 'https://' . variable_get('asu_comrf_degree_search_url_dev');
  } else if ($asu_comrf_degree_search_env == 'prod') {
    $url = 'https://' . variable_get('asu_comrf_degree_search_url_prod');
  }

  // Compose XML call
  $asu_comrf_degree_search_service_method = variable_get('asu_comrf_degree_search_service_method', 'findAllDegreesWithLessFields');

  //DEBUG in PROD
  $url = 'https://webapp4-qa.asu.edu/programs/XmlRpcServer';
  //$asu_comrf_degree_search_service_method = 'findAllDegrees';

  $xml_service_method = 'eAdvisorDSFind.' . $asu_comrf_degree_search_service_method;
  $xml_next_term_code = asu_comrf_get_next_term_code(); // Get next term code
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
    $test_number = 1;
    foreach($degree_program as $key => $value) {
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

      $campus_string_array_name = $programs_list_obj->value[$degree_key]->struct->member[$campus_string_array_result_index]->name;
      $campus_string_array_value = array();
      $campus_string_array_value_raw = $programs_list_obj->value[$degree_key]->struct->member[$campus_string_array_result_index]->value->array->data->value;

      $subplans_name = $programs_list_obj->value[$degree_key]->struct->member[$subplans_array_result_index]->name;
      $subplans_value = $programs_list_obj->value[$degree_key]->struct->member[$subplans_array_result_index]->value->struct->member;

        // DEBUG
      if (!is_null($subplans_value) && $test_number == 1) {
        $test_number++;
        watchdog('asu_comrf', var_dump($acad_plan_value) . "" . var_dump($subplans_value), array(), WATCHDOG_INFO);
      }

      // Campus array
      if (is_array($campus_string_array_value_raw)) {
        foreach($campus_string_array_value_raw as $key2 => $value2) {
          $campus_string_array_value[$key2] = $value2;
        }
      } else {
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
    }
  }

  // Sort the degree program list by the TrnscrDescr value
  usort($list_of_degrees, 'asu_comrf_sort_degree_list_by_trnscr_descr');

  // DEBUG
  // watchdog('asu_comrf', var_dump($list_of_degrees), array(), WATCHDOG_INFO);

  return $list_of_degrees;
}

/**
 * Sorts the degree list by TrnscrDescr value (e.g. degree program nice name).
 *
 * @param $a
 * @param $b
 * @return int
 */
function asu_comrf_sort_degree_list_by_trnscr_descr($a, $b) {
  return strcmp($a["TrnscrDescr"], $b["TrnscrDescr"]);
}

/**
 * Inserts an incoming list of degree programs from Degree Search into the database.
 *
 * @param $list_of_degrees
 * @return bool
 */
function asu_comrf_store_degree_list($list_of_degrees) {
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

  // If inserting data was successful, set check variable to true
  if ($stored_data !== NULL) {
    $data_stored_successfully = TRUE;
  }

  return $data_stored_successfully;
}