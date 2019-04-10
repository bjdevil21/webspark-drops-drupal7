(function ($) {
  Drupal.behaviors.asu_comrf_ds_gray_bar = {
    attach : function(context, settings) {
      // If page contains '.page-changingmajors-request' class AND does not contains the classes '.page-changingmajors-request-error' or '.page-changingmajors-request-confirmation'
      if($(".page-changingmajors-request")[0]) {
        if (!$(".page-changingmajors-request-notice")[0] && !$(".page-changingmajors-request-confirmation")[0]) {
          // Inject offset div for Degree Search bar
          var full_width_ds_bar_offset = '<div class="asu-comrf-full-width-ds-bar-offset"></div>';
          $(document).one("ready", function() {
            $('#block-asu-comrf-asu-comrf-student-info').before(full_width_ds_bar_offset);
          });
        }
      }
    }
  }
}(jQuery));