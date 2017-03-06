<?php

/**
 * @file
 * Bartik's theme implementation to display a single Drupal page.
 *
 * The doctype, html, head, and body tags are not in this template. Instead
 * they can be found in the html.tpl.php template normally located in the
 * core/modules/system directory.
 *
 * Available variables:
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/bartik.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 * - $
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or
 *   prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $site_name: The name of the site, empty when display has been disabled
 *   in theme settings.
 * - $site_slogan: The slogan of the site, empty when display has been disabled
 *   in theme settings.
 * - $hide_site_name: TRUE if the site name has been toggled off on the theme
 *   settings page. If hidden, the "element-invisible" class is added to make
 *   the site name visually hidden, but still accessible.
 * - $hide_site_slogan: TRUE if the site slogan has been toggled off on the
 *   theme settings page. If hidden, the "element-invisible" class is added to
 *   make the site slogan visually hidden, but still accessible.
 *
 * Navigation:
 * - $main_menu (array): An array containing the Main menu links for the
 *   site, if they have been configured.
 * - $main_menu_expanded (array): An array containing 2 depths of the Main menu links
 *   for the site, if they have been configured.
 * - $secondary_menu (array): An array containing the Secondary menu links for
 *   the site, if they have been configured.
 * - $breadcrumb: The breadcrumb trail for the current page.
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title: The page title, for use in the actual HTML content.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 * - $messages: HTML for status and error messages. Should be displayed
 *   prominently.
 * - $tabs (array): Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links (array): Actions local to the page, such as 'Add menu' on
 *   the menu administration interface.
 * - $feed_icons: A string of all feed icons for the current page.
 * - $node: The node entity, if there is an automatically-loaded node
 *   associated with the page, and the node ID is the second argument
 *   in the page's path (e.g. node/12345 and node/12345/revisions, but not
 *   comment/reply/12345).
 *
 * Regions:
 * - $page['content']: The main content of the current page.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see template_process()
 * @see bartik_process_page()
 * @see html.tpl.php
 *
 * @ingroup themeable
 */
?>
<div id="page-wrapper"><div id="page">

  <!-- Page Header -->
  <header id="header">
  	<div class="container">
  		<div class="row">
  			<div class="column col-md-12">
					<?php print render($page['header']); ?>
					<?php if ($site_name): ?>
                      <div class="header__sitename<?php if ($hide_site_name) { print ' element-invisible'; } ?>"><span><?php print $site_name; ?></span>
                      </div>
					<?php endif; ?>
				</div>
			</div>
		</div>
  </header><!-- /.header -->


	<!-- Nav Bar -->
	<div id="ASUNavMenu" class="navmenu">
		<div class="container">
      <!--Commented to work with mega menu-->
                     <nav class="navbar-collapse collapse">
					<?php print render($page['menu']); ?>
			</nav><!-- /#navbar -->
		</div><!-- /.container -->
	</div><!-- /.navmenu -->

    <!-- Page Main -->
  <div id="main-wrapper" class="clearfix"><div id="main" class="clearfix">
    <a id="main-content"></a>

    <div id="top-content" class="column container">
      <?php if (($no_panels || $always_show_page_title) && $title): ?>
        <h1 id="page-title" class="title">
          <?php print $title; ?>
        </h1>
      <?php endif; ?>

      <?php if ($messages): ?>
        <div id="messages">
          <?php print $messages; ?>
        </div>
      <?php endif; ?>

      <?php if ($tabs): ?>
        <div id="tabs">
          <?php print render($tabs); ?>
        </div>
      <?php endif; ?>

      <?php if ($action_links): ?>
        <div id="action-links">
          <?php print render($action_links); ?>
        </div>
      <?php endif; ?>
    </div> <!-- /#top-content -->

		<div id="content" class="column <?php $no_panels ? print 'container' : ''; ?>">
			<div id="page-content">
				<?php print render($page['content']); ?>
			</div><!-- /#page-content -->
		</div><!-- /#content -->

  </div></div> <!-- /#main, /#main-wrapper -->
  
  <!-- BoldChat Visitor Monitor HTML v5.00 (Website=My Website,Ruleset=My Invite Ruleset,Floating Chat=- None - -->
<script type="text/javascript">
  window._bcvma = window._bcvma || [];
  _bcvma.push(["setAccountID", "421005731063414730"]);
  _bcvma.push(["setParameter", "WebsiteID", "418316400453162226"]);
  _bcvma.push(["setParameter", "InvitationID", "418316400725203878"]);
  _bcvma.push(["pageViewed", document.location.href, document.referrer]);
  var bcLoad = function(){
    if(window.bcLoaded) return; window.bcLoaded = true;
    var vms = document.createElement("script"); vms.type = "text/javascript"; vms.async = true;
    vms.src = ('https:'==document.location.protocol?'https://':'http://') + "vmss.boldchat.com/aid/421005731063414730/bc.vms4/vms.js";
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(vms, s);
  };
  if(window.pageViewer && pageViewer.load) pageViewer.load();
  else if(document.readyState=="complete") bcLoad();
  else if(window.addEventListener) window.addEventListener('load', bcLoad, false);
  else window.attachEvent('onload', bcLoad);
</script>
<noscript>
<a href="http://www.boldchat.com" title="Live Chat" target="_blank"><img alt="Live Chat" src="https://vms.boldchat.com/aid/421005731063414730/bc.vmi?&amp;wdid=418316400453162226" border="0" width="1" height="1" /></a>
</noscript>
<!-- /BoldChat Visitor Monitor HTML v5.00 -->

  <!-- Page Footer -->
  <footer id="page-footer">
    <div class="container">
      <div class="row row-full">
      	<?php print render($page['footer']); ?>
      </div>
    </div>
  </footer><!-- /#footer -->

  <?php print render($page['closure']); ?>

  </div>
</div>
<!-- /#page, /#page-wrapper -->
