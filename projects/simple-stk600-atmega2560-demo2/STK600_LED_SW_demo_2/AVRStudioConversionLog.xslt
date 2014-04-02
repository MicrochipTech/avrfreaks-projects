<?xml version="1.0" encoding="utf-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:msxsl='urn:schemas-microsoft-com:xslt'>
	<xsl:template match="/">
		<html>
			<head>
				<title>
          <xsl:value-of select="ConversionLog/@BrandName"/>
        </title>
				<style type="text/css">
          body
          {
          font-size:8pt;
          font-family:Verdana;
          }
          .success { background-color:#00CC00; color:#00CC00; margin-right:5px; }
          .failure { background-color:#CC0000; color:#CC0000; margin-right:5px; }
          .Info { background-color:#FFFFFF; }
          .Warning { background-color:#AAAA00; }
          .Error { background-color:#AA0000; }
          .projectCount { font-size:12pt; }
          .header { font-size:15pt; }
          .subheader { font-weight:bold; font-size:10pt; margin-top:15px; margin-bottom:15px;}
          .project { font-size: 10pt; margin:20px; }
          .GeneralFailureInformation { font-size: 10pt; margin:20px; }
        </style>
				<script type="text/javascript">
					function toggleVisibility(id)
					{
					var element = document.getElementById(id);
					var link = document.getElementById('link_' + id);
					link.innerHTML = element.style.display == "none" ? "Hide Logs" : "Show Logs";	
					element.style.display = element.style.display == "none" ? "block" : "none";
					}
				</script>
			</head>
			<body>
				<div class="header">
          <xsl:value-of select="ConversionLog/@BrandName"/>
        </div>
        <xsl:if test="count(//GeneralFailureInformation) = 0">
        <br />
				<span class="projectCount"> <xsl:value-of select="count(//Project)" /> </span> project(s) chosen for conversion.
        </xsl:if>
				<br />
				<span class="projectCount"> <xsl:value-of select="count(//Project[@status='Success'])" /> </span> project(s) successfully converted.
        <br />
				<span class="projectCount"> <xsl:value-of select="count(//Project[@status='SuccessWithErrors'])" /> </span> project(s) converted with errors.
        <br />
        <br />
        <xsl:apply-templates select="//GeneralFailureInformation" />
        <br />
        <xsl:apply-templates select="//Project" />
 			</body>
		</html>
	</xsl:template>
	<xsl:template match="Project">
		<div class="project">
				<span>
					<xsl:attribute name="class"><xsl:value-of select="@status" /></xsl:attribute>
					<xsl:text>|</xsl:text>
				</span>
				<xsl:value-of select="@name" /> <br />
				<a href="javascript:">
					<xsl:attribute name="id">link_<xsl:value-of select="@id" />
					</xsl:attribute>
					<xsl:attribute name="onclick">
						toggleVisibility('<xsl:value-of select="@id" />');
					</xsl:attribute>
					Show Logs
				</a>

			<div style="display:none;clear:both;margin-top:10px;">
				<xsl:attribute name="id"><xsl:value-of select="@id" /></xsl:attribute>
				<table>
					<xsl:for-each select="LogEntry">
						<tr style="font-family:Verdana;font-size:8pt;">
							<xsl:attribute name="class"><xsl:value-of select="Type" /></xsl:attribute>
							<td>
								<xsl:value-of select="Message" />
							</td>
							<td>
								<xsl:value-of select="DateTime" />
							</td>
						</tr>
					</xsl:for-each>
				</table>
			</div>
		</div>
	</xsl:template>
  <xsl:template match="GeneralFailureInformation">
    <div class="GeneralFailureInformation">
      <span>
        <xsl:attribute name="class">
          <xsl:value-of select="@status" />
        </xsl:attribute>
        <xsl:text>|</xsl:text>
      </span>
      <xsl:value-of select="@name" />
      <br />
      <a href="javascript:">
        <xsl:attribute name="id">link_<xsl:value-of select="@id" />
        </xsl:attribute>
        <xsl:attribute name="onclick">
          toggleVisibility('<xsl:value-of select="@id" />');
        </xsl:attribute>
        Show Logs
      </a>

      <div style="display:none;clear:both;margin-top:10px;">
        <xsl:attribute name="id">
          <xsl:value-of select="@id" />
        </xsl:attribute>
        <table>
          <xsl:for-each select="LogEntry">
            <tr style="font-family:Verdana;font-size:8pt;">
              <xsl:attribute name="class">
                <xsl:value-of select="Type" />
              </xsl:attribute>
              <td>
                <xsl:value-of select="Message" />
              </td>
              <td>
                <xsl:value-of select="DateTime" />
              </td>
            </tr>
          </xsl:for-each>
        </table>
      </div>
    </div>
  </xsl:template>
</xsl:stylesheet>
