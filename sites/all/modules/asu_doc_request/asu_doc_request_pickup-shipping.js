(function ($) {
  Drupal.behaviors.asu_doc_request = {
    attach: function (context, settings) {

    // If a form change occurs in paper transcripts shipping service radios.
      // Hide shipping information if pickup options are selected.     
      $('#edit-commerce-shipping-shipping-service-commerce-pickup-node-27189').change(function() {
        $('fieldset#edit-customer-profile-shipping').hide();
      });

      $('#edit-commerce-shipping-shipping-service-commerce-pickup-node-27190').change(function() {
        $('fieldset#edit-customer-profile-shipping').hide();
      });

      $('#edit-commerce-shipping-shipping-service-us-mail').change(function() {
        $('fieldset#edit-customer-profile-shipping').show();
      });
      
      $('#edit-commerce-shipping-shipping-service-us-postal-express-mail').change(function() {
        $('fieldset#edit-customer-profile-shipping').show();
      });
      
      $('#edit-commerce-shipping-shipping-service-fedex-express').change(function() {
        $('fieldset#edit-customer-profile-shipping').show();
      });
      
      $('#edit-commerce-shipping-shipping-service-international-fedex-express').change(function() {
        $('fieldset#edit-customer-profile-shipping').show();
      });
      
      $('#edit-commerce-shipping-shipping-service-international-express-mail').change(function() {
        $('fieldset#edit-customer-profile-shipping').show();
      });
      
    }
  }
}(jQuery));
