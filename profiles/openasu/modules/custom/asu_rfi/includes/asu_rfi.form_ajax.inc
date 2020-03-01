<?php

/**
 * Change of Major request form AJAX callback used to update specific AJAX fields listed below.
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function asu_rfi_campus_ajax_callback($form, &$form_state) {
  dpm($form);
  $commands = array(
    ajax_command_replace("#asu-rfi-start-date-ajax-wrapper .form-item-start-date", '<h3>' . render($form['my_interest']['start_date']) . '</h3>'),
  );
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}
