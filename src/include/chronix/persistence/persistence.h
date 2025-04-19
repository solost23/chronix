#pragma once 

#include <vector> 

#include "chronix/define.h"

// file json
#include <fstream>
#include "nlohmann/json.hpp" 

// database mysql
#include <vector> 
#include "mysqlx/xdevapi.h"

/*
 * persistence base
 */
template<typename T> 
class Persistence 
{
public:
    virtual ~Persistence() = default; 

    // load task
    virtual std::vector<T> load() = 0; 

    // save task
    virtual void save(const std::vector<T>& jobs) = 0; 


    virtual std::string to_string(JobStatus status) 
    {
        switch (status) {
            case JobStatus::Pending: return "Pending";
            case JobStatus::Running: return "Running";
            case JobStatus::Paused:  return "Paused";
        }
        return "Pending";
    }

    virtual std::string to_string(JobResult result) 
    {
        switch (result) {
            case JobResult::Success: return "Success";
            case JobResult::Failed:  return "Failed";
            case JobResult::Unknown: return "Unknown";
        }
        return "Unknown";
    }

    virtual JobStatus from_string_status(const std::string& s) 
    {
        if (s == "Running") return JobStatus::Running;
        if (s == "Paused") return JobStatus::Paused;
        return JobStatus::Pending;
    }

    virtual JobResult from_string_result(const std::string& s) 
    {
        if (s == "Success") return JobResult::Success;
        if (s == "Failed") return JobResult::Failed;
        return JobResult::Unknown;
    }
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

    std::vector<T> load() override
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

    void save(const std::vector<T>& jobs) override
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
            {"status", this->to_string(job.status)},
            {"result", this->to_string(job.result)}
        }; 
    }

    T deserialize(const nlohmann::json& j)
    {
        T job; 
        job.id = j.at("id").get<int>(); 
        job.expr_str = j.at("expr").get<std::string>();
        job.expr = cron::make_cron(job.expr_str);
        job.status = this->from_string_status(j.value("status", "Pending"));
        job.result = this->from_string_result(j.value("result", "Unknown"));
        job.next = cron::cron_next(job.expr, std::chrono::system_clock::now());
        return job; 
    }
    
}; 

/*
 * persistence database
 * Description: support mysql
 */
template<typename T> 
class DBPersistenceMySQL : public Persistence<T> 
{
public:
    DBPersistenceMySQL(const std::string& host, 
                                int port, 
                                const std::string& user,
                                const std::string& password, 
                                const std::string& schema) : session(host, port, user, password), db(session.getSchema(schema)) {}
                
    std::vector<T> load() override
    {
        std::vector<T> jobs; 
        mysqlx::Table table = db.getTable("jobs");
        mysqlx::RowResult rows = table.select("id", "expr", "status", "result").execute(); 
        
        for (auto row : rows)
        {
            T job; 
            job.id = row[0]; 
            job.expr_str = row[1].get<std::string>(); 
            job.expr = cron::make_cron(job.expr_str);
            job.status = this->from_string_status(row[2].get<std::string>());
            job.result = this->from_string_result(row[3].get<std::string>());
            job.next = cron::cron_next(job.expr, std::chrono::system_clock::now());
            jobs.push_back(job);
        }

        return jobs; 
    }

    void save(const std::vector<T>& jobs) override
    {
        mysqlx::Table table = db.getTable("jobs"); 
        table.remove().execute(); 

        for (const auto& job : jobs)
        {
            table.insert("id", "expr", "status", "result")
                .values(job.id, job.expr_str, this->to_string(job.status), this->to_string(job.result))
                .execute();
        }
    }

private:
    mysqlx::Session session;  
    mysqlx::Schema db; 
}; 
