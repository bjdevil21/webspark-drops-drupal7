(function ($) {
  Drupal.behaviors.asu_comrf_ds_gray_bar = {
    attach : function(context, settings) {
      // If page contains '.page-request' class AND does not contains the classes '.page-request-notice' or '.page-request-confirmation'
      if($(".page-request")[0]) {
        if (!$(".page-request-notice")[0] && !$(".page-request-confirmation")[0]) {
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