// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <sstream>

#include "utils/json/json_parser.h"
#include "utils/unit_test/test_collector.h"


using namespace utl;

const char* test_json = R"({
  "web-app": {
    "servlet": [
      {
        "servlet-name": "cofaxCDS",
        "servlet-class": "org.cofax.cds.CDSServlet",
        "init-param": {
          "configGlossary:installationAt": "Philadelphia, PA",
          "configGlossary:adminEmail": "abc@def.com",
          "configGlossary:poweredBy": "Cofax",
          "configGlossary:poweredByIcon": "/images/cofax.gif",
          "configGlossary:staticPath": "/content/static",
          "templateProcessorClass": "org.cofax.WysiwygTemplate",
          "templateLoaderClass": "org.cofax.FilesTemplateLoader",
          "templatePath": "templates",
          "templateOverridePath": "",
          "defaultListTemplate": "listTemplate.htm",
          "defaultFileTemplate": "articleTemplate.htm",
          "useJSP": false,
          "jspListTemplate": "listTemplate.jsp",
          "jspFileTemplate": "articleTemplate.jsp",
          "cachePackageTagsTrack": 200,
          "cachePackageTagsStore": 200,
          "cachePackageTagsRefresh": 60,
          "cacheTemplatesTrack": 100.25,
          "cacheTemplatesStore": 50e10,
          "cacheTemplatesRefresh": 15,
          "cachePagesTrack": 200,
          "cachePagesStore": 100,
          "cachePagesRefresh": 10,
          "cachePagesDirtyRead": 10,
          "searchEngineListTemplate": "forSearchEnginesList.htm",
          "searchEngineFileTemplate": "forSearchEngines.htm",
          "searchEngineRobotsDb": "WEB-INF/robots.db",
          "useDataStore": true,
          "dataStoreClass": "org.cofax.SqlDataStore",
          "redirectionClass": "org.cofax.SqlRedirection",
          "dataStoreName": "cofax",
          "dataStoreDriver": "com.a1b2c3.jdbc.sqlserver.SQLServerDriver",
          "dataStoreUrl": "jdbc:a1b2c3:sqlserver://LOCALHOST:1433;DatabaseName=goon",
          "dataStoreUser": "sa",
          "dataStorePassword": "dataStoreTestQuery",
          "dataStoreTestQuery": "SET NOCOUNT ON;select test='test';",
          "dataStoreLogFile": "/usr/local/tomcat/logs/datastore.log",
          "dataStoreInitConns": 10,
          "dataStoreMaxConns": 100,
          "dataStoreConnUsageLimit": 100,
          "dataStoreLogLevel": "debug",
          "maxUrlLength": 500
        }
      },
      {
        "servlet-name": "cofaxEmail",
        "servlet-class": "org.cofax.cds.EmailServlet",
        "init-param": {
          "mailHost": "mail1",
          "mailHostOverride": "mail2"
        }
      },
      {
        "servlet-name": "cofaxAdmin",
        "servlet-class": "org.cofax.cds.AdminServlet"
      },
      {
        "servlet-name": "fileServlet",
        "servlet-class": "org.cofax.cds.FileServlet"
      },
      {
        "servlet-name": "cofaxTools",
        "servlet-class": "org.cofax.cms.CofaxToolsServlet",
        "init-param": {
          "templatePath": "toolstemplates/",
          "log": 1,
          "logLocation": "/usr/local/tomcat/logs/CofaxTools.log",
          "logMaxSize": "",
          "dataLog": 1,
          "dataLogLocation": "/usr/local/tomcat/logs/dataLog.log",
          "dataLogMaxSize": "",
          "removePageCache": "/content/admin/remove?cache=pages&id=",
          "removeTemplateCache": "/content/admin/remove?cache=templates&id=",
          "fileTransferFolder": "/usr/local/tomcat/webapps/content/fileTransferFolder",
          "lookInContext": 1,
          "adminGroupID": 4,
          "betaServer": true
        }
      }
    ],
    "servlet-mapping": {
      "cofaxCDS": "/",
      "cofaxEmail": "/cofaxutil/aemail/*",
      "cofaxAdmin": "/admin/*",
      "fileServlet": "/static/*",
      "cofaxTools": "/tools/*"
    },
    "taglib": {
      "taglib-uri": "cofax.tld",
      "taglib-location": "/WEB-INF/tlds/cofax.tld"
    }
  }
})";

TEST_CASE(JSONUnitTest) {

    TEST_DEF("JSON parser test.") {
        std::istringstream iss(std::string(test_json), std::ios::binary);
        if (!iss) {
            return false;
        }

        JSONParser parser;
        JSONParser::ValuePtr value;
        if (!parser.parse(iss, &value)) {
            return false;
        }

        auto object = value->asObject();
        json::ArrayValue* list_val;
        if (!object->getArray("web-app.servlet", &list_val)) {
            return false;
        }

        for (size_t i = 0; i < list_val->getCount(); ++i) {
            json::ObjectValue* item_val;
            if (!list_val->getObject(i, &item_val)) {
                continue;
            }
        }
        return true;
    };

}