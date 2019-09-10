/**
 * Webspark Callout JS
 */

(function ($, Drupal) {
  Drupal.behaviors.webspark_callout = {
    attach: function (context, settings) {
      $(".pane-bundle-callout").wrapInner("<div class='callout-wrapper'></div>");
      $(".pane-bundle-callout").addClass("callout-bg-img");

      $(function () {
        $('.form-item-field-webspark-callout-bgimg-und-0 :file').change(function () {

          if (this.files && this.files[0]) {
            var reader = new FileReader();
            reader.onload = imageIsLoaded;
            reader.readAsDataURL(this.files[0]);
          }
        });
      });

      // Adds background image to preview
      function imageIsLoaded(e) {
        $("#panopoly-form-widget-preview").before("<style> .callout-bg-img {background-image: url(" + e.target.result + ");}</style>");

      }
    }
  };
})(jQuery, Drupal);
