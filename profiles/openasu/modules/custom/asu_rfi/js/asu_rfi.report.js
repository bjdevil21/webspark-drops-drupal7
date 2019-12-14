(function ($) {
  Drupal.behaviors.asuRfiReport = {
    attach: function (context, settings) {
      $('fieldset.collapsible.asu-rfi-intro', context).once('asuRfiReport', function () {
        $(this).on("click", function() {
          Drupal.toggleFieldset(this);
        });
     });
    }
  }
})(jQuery);
