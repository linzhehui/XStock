#include "environment.h"
#include <webdriverxx/browsers/chrome.h>
#include <webdriverxx/browsers/firefox.h>
#include <webdriverxx/browsers/ie.h>
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>
#include <string>


namespace test {

using namespace webdriverxx;
Environment* Environment::instance_ = 0;

bool IsCommandLineArgument(const std::string& arg, const char* name) {
	return arg.find(std::string("--") + name) == 0;
}

std::string GetCommandLineArgumentValue(const std::string& arg) {
	const size_t pos = arg.find('=');
	return pos == std::string::npos ? std::string() : arg.substr(pos + 1);
}

Parameters ParseParameters(int argc, char** argv) {
	Parameters result;
	for (int i = 1; i < argc; ++i) {
		const std::string arg = argv[i];
		if (IsCommandLineArgument(arg, "browser")) {
			result.web_driver_url = webdriverxx::kDefaultWebDriverUrl;
			const std::string browser_name = GetCommandLineArgumentValue(arg);
			result.desired.Set("browserName", browser_name);
		}
		else if (IsCommandLineArgument(arg, "pages")) {
			result.test_pages_url = GetCommandLineArgumentValue(arg);
		}
		else if (IsCommandLineArgument(arg, "webdriver")) {
			result.web_driver_url = GetCommandLineArgumentValue(arg);
		}
		else if (IsCommandLineArgument(arg, "test_real_browsers")) {
			result.test_real_browsers = true;
		}
	}
	return result;
}

} // namespace test

#include <webdriverxx/browsers/ie.h>
int main(int argc, char** argv) {

	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(
		new test::Environment(test::ParseParameters(argc, argv))
	);
	return RUN_ALL_TESTS();
}
namespace test {

TEST(Firefox, WithTheSimplestSyntax) {
	if (!TestRealBrowsers()) return;
	auto ff = Start(Firefox());
}

TEST(Firefox, WithCustomUrl) {
	if (!TestRealBrowsers()) return;
	auto ff = Start(Firefox(), kDefaultWebDriverUrl);
}

TEST(Firefox, WithDefaultCapabilities) {
	if (!TestRealBrowsers()) return;
	auto defaults = Capabilities().SetProxy(DirectConnection());
	auto ff = Start(Firefox(defaults));
}

TEST(Firefox, HasCapabilitiesProperties) {
	if (!TestRealBrowsers()) return;
	auto ff = Start(Firefox().SetProxy(DirectConnection()));
}

TEST(Firefox, ConvertsToJson) {
	//foptions.SetLoggingPrefs(LoggingPrefs().SetDriverLogLevel(log_level::Warn));
	auto ff = Firefox();
	ff.GetFirefoxOptions().SetLoggingPrefs(LoggingPrefs().SetDriverLogLevel(log_level::Warn));
	//foptions.SetLoggingPrefs(LoggingPrefs().SetDriverLogLevel(log_level::Warn));

	const auto json = ToJson(ff);
	const auto c = FromJson<Capabilities>(json);
	const auto logging = c.Get<JsonObject>("loggingPrefs");

	ASSERT_EQ(browser::Firefox, c.GetBrowserName());
	ASSERT_EQ("WARNING", logging.Get<std::string>("driver"));
	ASSERT_EQ("abc", c.Get<std::string>("firefox_binary"));
}

TEST(InternetExplorer, ConvertsToJson) {
	auto ie = InternetExplorer();
	const auto json = ToJson(ie);
	const auto c = FromJson<Capabilities>(json);
	ASSERT_EQ(browser::InternetExplorer, c.GetBrowserName());
}

TEST(Chrome, ConvertsToJson) {
	auto gc = Chrome();
	const auto json = ToJson(gc);
	const auto c = FromJson<Capabilities>(json);
	ASSERT_EQ(browser::Chrome, c.GetBrowserName());
}

} // namespace test
