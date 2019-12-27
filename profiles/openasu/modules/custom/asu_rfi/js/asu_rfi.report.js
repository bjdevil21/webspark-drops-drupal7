(function ($) {
  Drupal.behaviors.asuRfiReport = {
    attach: function (context, settings) {
      $('fieldset.collapsible.asu-rfi-intro', context).once('asuRfiReport', function () {
        $(this).on("click", function() {
          Drupal.toggleFieldset(this);
        });
      });
      $('.view-display-id-attachment_1 h4.questions', context).once('asuRfiReport', function() {
        $(this).on("click", function() {
          $(this).siblings('.questions-container').toggle();
        });
      });
      $('.view-asu-rfi-submissions-report .view-header p:empty, .view-header br').remove();
    }
  }
})(jQuery);
