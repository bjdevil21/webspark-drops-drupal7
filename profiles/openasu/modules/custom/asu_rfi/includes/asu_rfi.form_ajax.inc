<?php

/**
 * Change of Major request form AJAX callback used to update specific AJAX fields listed below.
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function _asu_rfi_campus_ajax_callback($form, &$form_state) {
  $commands = array(
    dpm($form['my_interests'], __FUNCTION__ . ' form my interests'),
    dpm($form_state, __FUNCTION__ . ' form_state'),
    ajax_command_replace("#asu-rfi-start-date-ajax-wrapper", render($form['my_interests']['start_date']),
    ajax_command_replace("#asu-rfi-campus-ajax-wrapper", "<h1>HEY</h1>")),
  );
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}
