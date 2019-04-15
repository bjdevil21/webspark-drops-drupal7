<?php

/**
 * Block callback for asu_comrf_form_header.
 *
 * @return array
 */
function asu_comrf_form_header_BLOCK_CONTENT() {
  // Initialize block array
  $block = array();

  // Create $block_content variable with the COM form header and accompanying text
  $block_content = variable_get('asu_comrf_form_header_block_markup', '<div>There was an issue loading the Change of Major form header block content.</div>');

  // Define the block to return, injecting $block_content into the markup
  $block = array(
    'message' => array(
      '#prefix' => '',
      '#type' => 'markup',
      '#markup' => $block_content,
      '#suffix' => '',
    ),
  );

  return $block;
}

/**
 * Block callback for asu_comrf_degree_search.
 *
 * @return array
 */
function asu_comrf_degree_search_BLOCK_CONTENT() {
  // Initialize block array
  $block = array();

  // Create $block_content variable with the limited Degree Search bar markup
  $block_content = variable_get('asu_comrf_degree_search_block_markup', '<div>There was an issue loading the Change of Major Degree Search block content.</div>');

  // Define the block to return, injecting $block_content into the markup
  $block = array(
    'message' => array(
      '#prefix' => '',
      '#type' => 'markup',
      '#markup' => $block_content,
      '#suffix' => '',
    ),
  );

  return $block;
}

/**
 * Block callback for asu_comrf_student_info.
 *
 * @return array
 */
function asu_comrf_student_info_BLOCK_CONTENT($asurite_id) {
  // Initialize block array
  $block = array();

  // Loading helper function
  module_load_include('inc','asu_comrf','includes/asu_comrf.program_data_helpers');

  // Get student info via PeopleSoft data
  $student_info = asu_comrf_get_student_info($asurite_id);
  $student_name = $student_info['name']; // Format: Alison Smith
  $student_degrees = $student_info['degrees']; // Format: Accountancy, Business (Corporate Accounting) & Management
  $student_degrees_list = '';

  // Decide how many undergraduate degrees the student is associated with
  // If only 1 degree, assign degree data to the list variable for output
  if (count($student_degrees) == 1) {
    $student_degree = $student_degrees[0]['plan_name'];
    $student_degrees_list = (string)$student_degree;
  }
  // If more than 1 degree, iterate through degrees and assign degree data to the list variable for output
  if (count($student_degrees) > 1) {
    end($student_degrees);
    $last_degree_key = key($student_degrees);
    reset($student_degrees);
    $first_degree_key = key($student_degrees);
    foreach ($student_degrees as $key => $degree) {
      if ($key == $first_degree_key) {
        $student_degrees_list = '';
      }
      if ($key == $last_degree_key) {
        $student_degrees_list .= '& ' . $degree;
        break;
      } elseif ($key == $last_degree_key-1) {
        $student_degrees_list .= $degree . ' ';
      } else {
        $student_degrees_list .= $degree . ', ';
      }
    }
  }

  // Create $block_content variable with the student's name and accompanying degree programs
  $block_content = '<h2 class="asu-comrf-student-info-name">' . $student_name . '</h2>' .
    '<div class="asu-comrf-student-info-degrees">' . $student_degrees_list . '</div>';

  // Define the block to return, injecting $block_content into the markup
  $block = array(
    'message' => array(
      '#prefix' => '',
      '#type' => 'markup',
      '#markup' => $block_content,
      '#suffix' => '',
    ),
  );
  return $block;
}