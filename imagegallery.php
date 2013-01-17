<?php include("includes/head.html"); ?>
  <?php include("includes/header.html"); ?>
  <?php include("includes/sidebar.html"); ?>
  
  <!-- Add jQuery library --> 
  <script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7/jquery.min.js"></script> 
  
  <!-- Add mousewheel plugin (this is optional) --> 
  <script type="text/javascript" src="fancybox/lib/jquery.mousewheel-3.0.6.pack.js"></script> 
  
  <!-- Add fancyBox -->
  <link rel="stylesheet" href="fancybox/source/jquery.fancybox.css?v=2.1.3" type="text/css" media="screen" />
  <script type="text/javascript" src="fancybox/source/jquery.fancybox.pack.js?v=2.1.3"></script> 
  
  <!-- Optionally add helpers - button, thumbnail and/or media -->
  <link rel="stylesheet" href="fancybox/source/helpers/jquery.fancybox-buttons.css?v=1.0.5" type="text/css" media="screen" />
  <script type="text/javascript" src="fancybox/source/helpers/jquery.fancybox-buttons.js?v=1.0.5"></script> 
  <script type="text/javascript" src="fancybox/source/helpers/jquery.fancybox-media.js?v=1.0.5"></script>
  <link rel="stylesheet" href="fancybox/source/helpers/jquery.fancybox-thumbs.css?v=1.0.7" type="text/css" media="screen" />
  <script type="text/javascript" src="fancybox/source/helpers/jquery.fancybox-thumbs.js?v=1.0.7"></script> 

  <script type="text/javascript">
$(document).ready(function() {
	$(".fancybox-thumb").fancybox({
		prevEffect	: 'none',
		nextEffect	: 'none',
		helpers	: {
			title	: {
				type: 'outside'
			},
			thumbs	: {
				width	: 50,
				height	: 50
			}
		}
	});
});
</script>
  <div class="content">
    <p>Awesome SLIPC at work </p>
    <?php
$images = glob("images/small/{*.jpg,*.gif,*.png,*.JPG}", GLOB_BRACE);

 

foreach ($images as $pic){?>
    <a class="fancybox-thumb" rel="fancybox-thumb" href="/teaching/slipc12-13/<?php echo $pic; ?>"><img src="/teaching/slipc12-13/<?php echo $pic; ?>"  width="175" height="119" alt="" /></a>
    <?php
}
?>
  </div>
  <?php include("includes/footer.html"); ?>
