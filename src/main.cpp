/**
 * @file main.cpp
 * @brief Main entry point for souncdown CLI
 * @author SounCdown Team
 * @version 1.0.0
 */

#include "souncdown/logger.hpp"
#include "souncdown/process.hpp"
#include "souncdown/args_parser.hpp"
#include "souncdown/playlist.hpp"
#include "souncdown/downloader.hpp"
#include "souncdown/converter.hpp"
#include "souncdown/config.hpp"
#include "souncdown/progress_bar.hpp"

#include <iostream>
#include <cstdlib>

using namespace souncdown;

int main(int argc, char* argv[]) {
    try {
        // Parse command-line arguments
        auto parse_result = ArgsParser::parse(argc, argv);
        
        if (!parse_result) {
            std::cerr << "Error: " << parse_result.error() << "\n\n";
            ArgsParser::print_help(argv[0]);
            return EXIT_FAILURE;
        }
        
        auto args = *parse_result;
        
        // Enable progress bars if requested
        ProgressBar::set_enabled(args.show_progress);
        
        // Set log level based on verbosity
        // If progress is enabled, reduce logging to avoid interference
        if (args.show_progress && !args.verbose) {
            Logger::instance().set_level(LogLevel::WARN);
        } else if (args.verbose) {
            Logger::instance().set_level(LogLevel::DEBUG);
        } else if (args.quiet) {
            Logger::instance().set_level(LogLevel::WARN);
        } else {
            Logger::instance().set_level(args.options.log_level);
        }
        
        // Handle different command modes
        switch (args.mode) {
            case CommandMode::VERSION:
                ArgsParser::print_version();
                return EXIT_SUCCESS;
                
            case CommandMode::HELP:
                ArgsParser::print_help(argv[0]);
                ArgsParser::print_examples();
                return EXIT_SUCCESS;
                
            case CommandMode::CHECK_DEPS: {
                LOG_INFO("Checking dependencies...");
                auto dep_check = Process::check_dependencies();
                if (dep_check) {
                    LOG_INFO("All dependencies are installed");
                    return EXIT_SUCCESS;
                } else {
                    LOG_ERROR("{}", dep_check.error());
                    return EXIT_FAILURE;
                }
            }
            
            case CommandMode::INFO: {
                LOG_INFO("Analyzing URLs...\n");
                for (const auto& url : args.urls) {
                    auto info_result = Playlist::get_info(url, args.options.oauth_token);
                    if (info_result) {
                        Playlist::display_info(*info_result);
                    } else {
                        LOG_ERROR("Failed to get info for {}: {}", url, info_result.error());
                    }
                }
                return EXIT_SUCCESS;
            }
            
            case CommandMode::FORMATS: {
                for (const auto& url : args.urls) {
                    LOG_INFO("\nFormats for: {}", url);
                    LOG_INFO("{}", std::string(80, '='));
                    
                    auto formats_result = Playlist::get_available_formats(
                        url, 
                        args.options.oauth_token
                    );
                    
                    if (formats_result) {
                        for (const auto& format : *formats_result) {
                            std::cout << format << "\n";
                        }
                    } else {
                        LOG_ERROR("Failed to get formats: {}", formats_result.error());
                    }
                }
                return EXIT_SUCCESS;
            }
            
            case CommandMode::DOWNLOAD: {
                // Check dependencies first
                auto dep_check = Process::check_dependencies();
                if (!dep_check) {
                    LOG_ERROR("{}", dep_check.error());
                    return EXIT_FAILURE;
                }
                
                // Create downloader
                Downloader downloader(args.options);
                
                // Progress callback
                auto progress_callback = [](const DownloadProgress& progress) {
                    if (progress.total_tracks > 1) {
                        LOG_INFO("Progress: [{}/{}] {:.1f}%", 
                                progress.current_track,
                                progress.total_tracks,
                                progress.percentage);
                    }
                };
                
                // Start download
                LOG_INFO("Starting download...\n");
                auto download_result = downloader.download(args.urls, progress_callback);
                
                if (download_result) {
                    // Convert if needed
                    if (args.options.output_format != AudioFormat::M4A && 
                        args.options.output_format != AudioFormat::OPUS) {
                        LOG_INFO("\nConverting files to {}...", 
                                to_string(args.options.output_format));
                        
                        Converter converter(args.options.output_format);
                        auto convert_result = converter.convert_directory(
                            args.options.output_directory
                        );
                        
                        if (convert_result) {
                            LOG_INFO("Converted {} files", *convert_result);
                        } else {
                            LOG_WARN("Conversion had issues: {}", convert_result.error());
                        }
                    }
                    
                    LOG_INFO("\nAll done!");
                    return EXIT_SUCCESS;
                } else {
                    LOG_ERROR("Download failed: {}", download_result.error());
                    return EXIT_FAILURE;
                }
            }
        }
        
    } catch (const std::exception& e) {
        LOG_CRITICAL("Unexpected error: {}", e.what());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
