<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
        xmlns="http://www.w3.org/1999/xhtml"
        xmlns:xs="http://www.w3.org/2001/XMLSchema"
        xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
        xmlns:xlink="http://www.w3.org/1999/xlink" 
        xmlns:local="http://www.yoursite.org/namespace" 
        xmlns:ead="urn:isbn:1-931666-22-9" version="2.0"  exclude-result-prefixes="#all">
    <!-- THIS WORK IS A DERIVATIVE WORK OF https://github.com/archivesspace/archivesspace
         Originally published under the Educational Community License 2.0
         A copy of the license is found in /archivesspace/LICENSE and is available 
         at https://opensource.org/licenses/ecl2.php
    -->
    
    <!-- import the archivesspace transform -->
    <xsl:import href="archivesspace/as-ead-html.xsl"/>
   
    <xsl:output method="xhtml" indent="yes" encoding="UTF-8"/>
    
    <!-- override construction of html document, construct a div instead -->
    <!-- we don't build the /html/head/meta[@http-equiv = 'Content-Type'] dc elements -->
    <xsl:template match="/" exclude-result-prefixes="#all">
        <div class="finding-aid">
            <!-- this is gross and invalid, needs fixed by dev -->
            <div class="publication"><xsl:apply-templates select="ead:ead/ead:eadheader/ead:filedesc/ead:publicationstmt/ead:publisher"/></div>
                <div id="main">
                    <xsl:apply-templates select="ead:ead/ead:archdesc" mode="toc"/>
                    <xsl:apply-templates select="ead:ead/ead:archdesc"/>
                </div>
            </div>
    </xsl:template>
    
    <!--This is a named template that processes all the components  -->
    <!-- Modified to add td/@id that is used to create link to Islandora object from item listings (c) -->
    <xsl:template name="clevel">
        <!-- 
            Establishes which level is being processed in order to provided indented displays. 
            Indents handled by CSS margins
        -->
        <xsl:param name="level" />
        <xsl:variable name="clevelMargin">
            <xsl:choose>
                <xsl:when test="$level = 1">c01</xsl:when>
                <xsl:when test="$level = 2">c02</xsl:when>
                <xsl:when test="$level = 3">c03</xsl:when>
                <xsl:when test="$level = 4">c04</xsl:when>
                <xsl:when test="$level = 5">c05</xsl:when>
                <xsl:when test="$level = 6">c06</xsl:when>
                <xsl:when test="$level = 7">c07</xsl:when>
                <xsl:when test="$level = 8">c08</xsl:when>
                <xsl:when test="$level = 9">c09</xsl:when>
                <xsl:when test="$level = 10">c10</xsl:when>
                <xsl:when test="$level = 11">c11</xsl:when>
                <xsl:when test="$level = 12">c12</xsl:when>
            </xsl:choose>
        </xsl:variable>
        <xsl:choose>
            <!--Formats Series and Groups  -->
            <xsl:when test="@level='subcollection' or @level='subgrp' or @level='series' 
                or @level='subseries' or @level='collection'or @level='fonds' or 
                @level='recordgrp' or @level='subfonds' or @level='class' or (@level='otherlevel' and not(child::ead:did/ead:container))">
                <tr id="{local:buildID(.)}">
                    <xsl:attribute name="class">
                        <xsl:choose>
                            <xsl:when test="@level='subcollection' or @level='subgrp' or @level='subseries' or @level='subfonds'">subseries</xsl:when>
                            <xsl:otherwise>series</xsl:otherwise>
                        </xsl:choose>    
                    </xsl:attribute>
                    <th class="{$clevelMargin}" rowspan="{count(ead:did/ead:container[@label]) + 1}">
                        <xsl:if test="not(ead:did/ead:container)">
                            <xsl:attribute name="colspan">4</xsl:attribute>  
                        </xsl:if>
                        <xsl:apply-templates select="ead:did" mode="dscSeriesTitle"/>
                        <div class="dscSeries">
                            <xsl:apply-templates select="ead:did" mode="dscSeries"/>
                            <xsl:apply-templates select="child::*[not(ead:did) and not(self::ead:did)]" mode="dsc"/>
                        </div>
                    </th>
                </tr>
                <!-- Adds grouped instances if they exist -->
                <xsl:for-each-group select="ead:did/ead:container" group-starting-with="ead:container[@label]">
                    <tr>
                        <xsl:apply-templates select="current-group()" mode="series"/>
                        <xsl:choose>
                            <xsl:when test="count(current-group()) = 1">
                                <th><span class="containerType"/></th>
                                <th><span class="containerType"/></th>
                            </xsl:when>
                            <xsl:when test="count(current-group()) = 2">
                                <th><span class="containerType"/></th>
                            </xsl:when>
                            <xsl:when test="count(current-group()) = 3"/>
                        </xsl:choose>
                    </tr>
                </xsl:for-each-group>
                <!-- Adds table headers if child is item or file -->
                <xsl:if test="child::*[@level][1][@level = 'item' or @level='file' or @level = 'otherlevel']">
                    <xsl:call-template name="tableHeaders"/>
                </xsl:if>
            </xsl:when>
            <xsl:when test="ead:did/ead:container[@label]">
                <tr class="item">
                    <!-- modified to add @id -->
                    <td class="{$clevelMargin}" rowspan="{count(ead:did/ead:container[@label]) + 1}" id="{@id}">
                        <xsl:apply-templates select="ead:did" mode="dsc"/>  
                        <xsl:apply-templates mode="dsc" select="*[not(self::ead:did) and 
                            not(self::ead:c) and not(self::ead:c02) and not(self::ead:c03) and
                            not(self::ead:c04) and not(self::ead:c05) and not(self::ead:c06) and not(self::ead:c07)
                            and not(self::ead:c08) and not(self::ead:c09) and not(self::ead:c10) and not(self::ead:c11) and not(self::ead:c12)]"/>          
                    </td>
                </tr>
                <!-- Groups instances by label attribute, the way they are grouped in ArchivesSpace -->
                <xsl:for-each-group select="ead:did/ead:container" group-starting-with="ead:container[@label]">
                    <tr>
                        <xsl:apply-templates select="current-group()" />
                        <xsl:choose>
                            <xsl:when test="count(current-group()) &lt; 2">
                                <td><span class="containerType"/></td>
                                <td><span class="containerType"/></td>
                            </xsl:when>
                            <xsl:when test="count(current-group()) &lt; 3">
                                <td><span class="containerType"/></td>
                            </xsl:when>
                        </xsl:choose>
                    </tr>
                </xsl:for-each-group>
            </xsl:when>
            <!-- For finding aids with no @label attribute, only accounts for three containers -->
            <xsl:otherwise>
                <tr class="item">
                    <td class="{$clevelMargin}" rowspan="{count(ead:did/ead:container[@label]) + 1}">
                        <xsl:apply-templates select="ead:did" mode="dsc"/>  
                        <xsl:apply-templates mode="dsc" select="*[not(self::ead:did) and 
                            not(self::ead:c) and not(self::ead:c02) and not(self::ead:c03) and
                            not(self::ead:c04) and not(self::ead:c05) and not(self::ead:c06) and not(self::ead:c07)
                            and not(self::ead:c08) and not(self::ead:c09) and not(self::ead:c10) and not(self::ead:c11) and not(self::ead:c12)]"/>          
                    </td>
                    <td>
                        <span class="containerType"><xsl:value-of select="ead:did/ead:container[1]/@type"/></span><xsl:text> </xsl:text><xsl:value-of select="ead:did/ead:container[1]"/>
                    </td>
                    <td class="container">
                        <span class="containerType"><xsl:value-of select="ead:did/ead:container[2]/@type"/></span><xsl:text> </xsl:text><xsl:value-of select="ead:did/ead:container[2]"/>
                    </td>
                    <td class="container">
                        <span class="containerType"><xsl:value-of select="ead:did/ead:container[3]/@type"/></span><xsl:text> </xsl:text><xsl:value-of select="ead:did/ead:container[3]"/>
                    </td>
                </tr>
            </xsl:otherwise>
        </xsl:choose>
        <!-- Calls child components -->
        <xsl:apply-templates select="ead:c | ead:c01 | ead:c02 | ead:c03 | ead:c04 | ead:c05 | ead:c06 | ead:c07 | ead:c08 | ead:c09 | ead:c10 | ead:c11 | ead:c12"/>
    </xsl:template>
    
    
</xsl:stylesheet>