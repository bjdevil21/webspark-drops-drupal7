<?php

/**
 * Determines when ASU is off-season.
 *
 * Returns TRUE if off-season, and returns FALSE if not off-season.
 *
 * @return bool
 */
function asu_comrf_is_off_season() {
  // Helper function to only allow query of all degree programs during academic off-seasons (summer, winter break)
  // if (month == 12 && day == 20-31) || (month == 5 && day == 10-31) || (month == 6-7) || (month == 8 && day == 1-15)
  //$current_day = date('j');
  $current_month = date('n');

  // Current months (by month digit) that are considered off-season
  switch($current_month) {
    case '5':
    case '6':
    case '7':
    case '8':
    case '12':
      return TRUE;
    default:
      break;
  }

  return FALSE;
}

/**
 * Determines if at least one week has passed since the last check time.
 *
 * Returns TRUE if at least one week has passed, and returns FALSE if one week has not passed since the last recorded check.
 *
 * @return bool
 */
function asu_comrf_at_least_one_week_has_passed() {
  // Defines integer constant, which is amount of seconds in a week
  define('WEEK_IN_SECONDS', 604800);

  // Creates a temporary variable that verifies time line
  $check_val = FALSE;

  // Gets difference between time stamp of last cron check and current time stamp
  $time_lapse = time() - variable_get('asu_comrf_check_time', WEEK_IN_SECONDS);

  // 1) Compares time stamp to check if last cron run was a week ago
  // 2) Set to TRUE if it has been over a week
  if ($time_lapse >= WEEK_IN_SECONDS) {
    $check_val = TRUE;
  }

  // Sets new timestamp as global variable
  variable_set('asu_comrf_check_time', time());

  // Returns TRUE if last cron was run more than a week ago
  return $check_val;
}

/**
 * Determines the next term code for Degree Search.
 *
 * @return string
 */
function asu_comrf_get_next_term_code() {
  // The first digit of '2' indicates that the year is at least 2000 ('0' was 19XX)
  // The second and third digits give you the rest of the year (so '213X' is 2013)
  // The fourth digit is semester.
  // '1' = spring
  // '7' = fall
  // '9' = winter
  // '4' = summer
  // '5' = summer session 2 (no longer used, but it might show up if we're pulling old enough data--we should probably just merge 5 with 4)

  // Set date-based variables
  $current_millennium = '2';
  $current_year = date('y');
  $current_month = date('n');

  // Initialize term identifier
  $term_identifier = '';

  // Determine term identifier
  switch($current_month) {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      $term_identifier = '7';
      break;
    case '8':
    case '9':
    case '10':
    case '11':
    case '12':
      $term_identifier = '1';
      $current_year += 1;
      break;
    default:
      break;
  }

  // Build term code
  $next_term = $current_millennium . $current_year . $term_identifier;

  return $next_term;
}

/**
 * Get individual student information from PeopleSoft data to display on form above fields.
 *
 * @return array
 */
function asu_comrf_get_student_info($asurite_id) {

  // Set Mulesoft client ID and secret
  $client_id = variable_get('asu_comrf_mulesoft_api_client_id');
  $client_secret = variable_get('asu_comrf_mulesoft_api_client_secret');
  $api_env = variable_get('asu_comrf_change_major_api_student_env');
  $api_url = variable_get('asu_comrf_change_major_api_student_url_' . $api_env);
  $student_info_base_url = 'https://' . $client_id . ':' . $client_secret . '@' . $api_url;
  $student_info_url = $student_info_base_url . '?id=' . $asurite_id;

  $mulesoft_auth = variable_get('asu_comrf_change_major_api_email_env', '');

  $request_options = array(
    'headers' => array(
      'Authorization' => $mulesoft_auth,
    ),
    'method' => 'GET',
  );

  $result = drupal_http_request($student_info_url, $request_options);

  if ($result->code <> 200) {
    watchdog('asu_comrf', __FUNCTION__ . ' - Mulesoft call URL (sans app_ID:key) for ASURITE ID ' . $asurite_id . ': ' . $api_url, array(), WATCHDOG_DEBUG);
    watchdog('asu_comrf', __FUNCTION__ . ' - Mulesoft call return code ' . $result->code, array(), WATCHDOG_ERROR);
  }

  // See https://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/Errors/unix_system_errors.html for error codes.
  if (($result->code > 0) && ($result->code < 125)) {
    watchdog('asu_comrf', __FUNCTION__ . ' - Mulesoft call for ASURITE ID ' . $asurite_id . ' failed with system error ' . $result->code
      . '. Check Mulesoft API to see if it is working.', array(), WATCHDOG_ERROR);
    drupal_goto("request/error");
    drupal_exit();
  } elseif (isset($result->error)) {
    watchdog('asu_comrf', __FUNCTION__ . ' - Mulesoft call for ASURITE ID ' . $asurite_id . ' not working. Returns error ' . check_plain($result->error),
      array(), WATCHDOG_ERROR);
    drupal_goto("request/notice");
    drupal_exit();
  }

  $student_info_raw = json_decode($result->data)->studentMajor;
  $student_info_raw = $student_info_raw[0];
  $student_info = asu_comrf_build_student_profile($student_info_raw);

  return $student_info;
}

/**
 * Builds the student profile for the student information block via asu_comrf_get_student_info().
 *
 * @param $student_info_raw
 * @return array
 */
function asu_comrf_build_student_profile($student_info_raw) {

  // Initialize array for degree programs
  $degree_programs = array();

  // Get student's name and careers to parse through
  $student_info['name'] = $student_info_raw->firstName . ' ' . $student_info_raw->lastName;
  $student_info['emplid'] = $student_info_raw->emplid;
  $careers = $student_info_raw->careers;

  if (empty($careers)) {
    drupal_goto("request/notice");
    drupal_exit();
  }

  // Initialize variable to count number of MAJ or PRE plan types for a given student
  $plan_type_program_count = 0;

  // Only display undergraduate majors
  foreach ($careers as $career) {
    if ($career->career == 'UGRD') {
      foreach ($career->plans as $plan) {
        if ($plan->planType == 'MAJ' || $plan->planType == 'PRE') {
          $plan_name = $plan->transcriptDescription;
          $degree_type = $plan->degree;
          $current_program = array(
            'plan_name' => $plan_name,
            'degree_type' => $degree_type,
          );
          array_push($degree_programs, $current_program);
          $plan_type_program_count++;
        }
      }
    }
  }

  // Send user to notice page if they don't have at least one MAJ or PRE plan type
  if ($plan_type_program_count == 0) {
    drupal_goto("request/notice");
    drupal_exit();
  }

  // Add MAJ plan type degree programs to student profile
  $student_info['degrees'] = $degree_programs;

  return $student_info;
}

/**
 * Returns the current degree program list from the database.
 *
 * @return mixed
 */
function asu_comrf_get_current_program_list() {
  // Retrieve degree program list from database and unserialize the data for processing
  $list_of_degrees_query = db_query("SELECT degree_programs FROM {asu_comrf_degree_list} ORDER BY lid DESC LIMIT 1");
  $list_of_degrees_results_array_s = $list_of_degrees_query->fetchAll();
  $list_of_degrees_results_s = $list_of_degrees_results_array_s[0]->degree_programs;
  $list_of_degrees_results = unserialize($list_of_degrees_results_s);

  // Check for malformed data returned from Degree Search (e.g. 'Degree' field is empty, therefore it defaults to an object, which causes errors down the line)
  if ($list_of_degrees_results) {
    for ($i = 0; $i < count($list_of_degrees_results); $i++) {
      if (is_object($list_of_degrees_results[$i]['TrnscrDescr'])) {
        $list_of_degrees_results[$i]['TrnscrDescr'] = '';
      }
      if (is_object($list_of_degrees_results[$i]['Degree'])) {
        $list_of_degrees_results[$i]['Degree'] = '';
      }
      if (is_object($list_of_degrees_results[$i]['AcadPlan'])) {
        $list_of_degrees_results[$i]['AcadPlan'] = '';
      }
    }
  }

  return $list_of_degrees_results;
}

/**
 * Translates Peoplesoft-esque campus codes into a human-readable format.
 *
 * @param $new_campus_choice_raw
 * @return array
 */
function asu_comrf_translate_campus_code_options($new_campus_choice_raw) {
  // Initialize campus choice array to return
  $new_campus_choice = array();
  // Translate campus codes to human-readable names
  foreach ($new_campus_choice_raw as $key => $campus_raw) {
    switch ($campus_raw) {
      case 'ONLNE':
        $new_campus_choice[$key] = 'Online';
        break;
      case 'DOWNTOWN':
      case 'DTPHX':
        $new_campus_choice[$key] = 'Downtown';
        break;
      case 'POLY':
        $new_campus_choice[$key] = 'Polytechnic';
        break;
      case 'TMPE':
      case 'TEMPE':
        $new_campus_choice[$key] = 'Tempe';
        break;
      case 'WEST':
        $new_campus_choice[$key] = 'West';
        break;
      case 'CALHC':
        $new_campus_choice[$key] = 'Lake Havasu City';
        break;
      case 'CAC':
        $new_campus_choice[$key] = 'ASU@Pinal';
        break;
      case 'EAC':
        $new_campus_choice[$key] = 'ASU@TheGilaValley';
        break;
      case 'TUCSN':
        $new_campus_choice[$key] = 'ASU@Tucson';
        break;
      case 'AWC':
        $new_campus_choice[$key] = 'ASU@Yuma';
        break;
      case 'TBIRD':
        $new_campus_choice[$key] = 'Thunderbird';
        break;
      case 'YAVAP':
        $new_campus_choice[$key] = 'ASU@Yavapai';
        break;
      case 'PIMA':
        $new_campus_choice[$key] = 'ASU@Pima';
        break;
      case 'COCHS':
        $new_campus_choice[$key] = 'ASU@Cochise';
        break;
      default:
        $new_campus_choice[$key] = $campus_raw;
        break;
    }
  }
  return $new_campus_choice;
}

/**
 * Helper function - Get all subplans for an academic plan,
 * and alter the form and form_state variables
 */
function _asu_comrf_render_subplans_list($list_of_subplans, &$form) {

  $new_subplan_options = array();

  // Does subplan actually exist (in a non-empty object for one option
  // or array for two or more options)?
  if ((is_object($list_of_subplans) && !empty($list_of_subplans->name)) ||
    (is_array($list_of_subplans) && isset($list_of_subplans[0]))) {

    if (is_object($list_of_subplans)) { // Only one subplan object
      $new_subplan_options[$list_of_subplans->name] = $list_of_subplans->value;
      watchdog('asu_com', __FUNCTION__ . ' is_object', array(), WATCHDOG_DEBUG);
    }
    else { // two or more subplans objects are wrapped by an array
      foreach($list_of_subplans as $sub => $plan) {
        $new_subplan_options[$plan->name] = $plan->value;
      }
      watchdog('asu_com', __FUNCTION__ . ' is_array', array(), WATCHDOG_DEBUG);
    }
    $form['asu_comrf_new_subplan_choice']['#options'] = $new_subplan_options;
    $form['asu_comrf_new_subplan_choice']['#required'] = TRUE;
  }
  else { // No subplans are available, so don't make any changes
    $form['asu_comrf_new_subplan_choice']['#options'] = array(0 => 'No subplans available');
    $form['asu_comrf_new_subplan_choice']['#required'] = FALSE;
    watchdog('asu_com', __FUNCTION__ . ' - list of subplans is EMPTY', array(), WATCHDOG_DEBUG);
  }
}