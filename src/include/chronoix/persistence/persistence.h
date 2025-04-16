#pragma once 

#include <vector> 

#include "chronoix/define.h"

// file json
#include <fstream>
#include "nlohmann/json.hpp" 


/*
 * persistence base
 */
template<typename T> 
class Persistence 
{
public:
    virtual ~Persistence() = default; 

    // load task
    virtual std::vector<T> load_jobs() = 0; 

    // save task
    virtual void save_job(const std::vector<T>& jobs) = 0; 
}; 

/*
 * persistence file 
 * Description: support json 
 */
template<typename T> 
class FilePersistenceJson : public Persistence<T> 
{
public:
    explicit FilePersistenceJson(const std::string& path) : filepath(path) {}

    std::vector<T> load_jobs() override
    {
        std::ifstream ifs(filepath); 
        if (!ifs.is_open())
        {
            return {}; 
        }

        nlohmann::json j;
        ifs >> j;

        std::vector<T> jobs;
        for (const auto& item : j)
        {
            jobs.push_back(deserialize(item)); 
        }
        return jobs; 
    }

    void save_job(const std::vector<T>& jobs) override
    {
        nlohmann::json j;
        for (const auto& job : jobs)
        {
            j.push_back(serialize(job)); 
        }

        std::ofstream ofs(filepath); 
        ofs << j.dump(4); 
    }

private:
    std::string filepath; 

    nlohmann::json serialize(const T& job)
    {
        return nlohmann::json{
            {"id", job.id}, 
            {"expr", job.expr_str}, 
            {"paused", job.paused}
        }; 
    }

    T deserialize(const nlohmann::json& j)
    {
        T job; 
        job.id = j.at("id").get<int>(); 
        job.expr_str = j.at("expr").get<std::string>();
        job.expr = cron::make_cron(job.expr_str);
        job.paused = j.at("paused").get<bool>(); 
        job.next = cron::cron_next(job.expr, std::chrono::system_clock::now());
        return job; 
    }
    
}; 

/*
 * persistence database
 * Description: support mysql
 * TOOD: 
 */
template<typename T> 
class PersistenceDatabase : public Persistence<T> 
{
public:
    explicit PersistenceDatabase(const std::string& conn_str);
    std::vector<Job> load_jobs() override;
    void save_job(const std::vector<Job>& jobs) override;

private:
    std::string conn_str;
}; 
