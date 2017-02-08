// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/Log.hpp"

namespace analyzer {
    namespace log {

        std::mutex log_mutex = { };
        static std::unordered_map<int32_t, std::string> errors;

        static void SetErrorStrings() noexcept;

        std::mutex StrSysError::error_mutex = { };
        StrSysError* volatile StrSysError::pInstance { nullptr };

        StrSysError* StrSysError::Instance()
        {
            if (pInstance == nullptr) {
                std::lock_guard<std::mutex> lock(error_mutex);
                if (pInstance == nullptr) {
                    SetErrorStrings();
                    pInstance = new StrSysError();
                }
            }
            return pInstance;
        }

        std::string StrSysError::operator() (const int32_t error) noexcept
        {
            error_mutex.lock();
            auto it = errors.find(error);
            error_mutex.unlock();

            if (it != errors.end()) { return (*it).second; }
            return std::string("Unknown error (" + std::to_string(error) + ").");
        }


        static inline std::string get_hex (const std::size_t value, const int32_t width = 16, bool is_upper = true)
        {
            std::ostringstream result;
            if (is_upper) { result.setf(std::ios_base::uppercase); }
            result << std::hex << std::setfill('0') << std::setw(width) << value;
            return result.str();
        }

        std::string __get_time_string() noexcept
        {
            using std::chrono::system_clock;
            time_t time = system_clock::to_time_t(system_clock::now());
            std::string curr_time = ctime(&time);
            return curr_time.erase(19, 6).erase(0, 11);
        }


        // Need to add three flag: is_offset, is_data, is_upper...
        // Need to add dependency length of the offset from the length of data ().
        void DbgHexDump (const char* message, void* data, std::size_t size, std::size_t line_length)
        {
            if (line_length % 2 == 1) { line_length++; }
            if (line_length < 8) { line_length = 8; }
            const std::size_t full_chunks = size / line_length;
            const std::size_t last_chunk = size % line_length;
            const std::size_t mean_length = line_length / 2;

            char* pSource = static_cast<char *>(data);
            const std::size_t hex_lines = full_chunks + (last_chunk == 0 ? 0 : 1) + 2;
            const std::size_t hex_dump_line_length = 11 + 4 * line_length + 8;
            const std::size_t hex_dump_size = hex_dump_line_length * hex_lines + 1;

            // Make hex dump header (2 lines).
            std::string hex_dump(hex_dump_size, ' ');
            hex_dump.replace(1, 8, "shift  |");
            for (std::size_t i = 0; i < line_length; ++i) {
                if (i < mean_length) { hex_dump.replace(12 + i * 3, 2, get_hex(i, 2)); }
                else { hex_dump.replace(13 + i * 3, 2, get_hex(i, 2)); }
            }
            hex_dump.replace(line_length * 3 + 17, 4, "data");
            hex_dump[hex_dump_line_length - 1] = '\n';
            hex_dump.replace(hex_dump_line_length, hex_dump_line_length - 1, hex_dump_line_length - 1, '-');
            hex_dump[hex_dump_line_length + 8] = '|';
            hex_dump[hex_dump_line_length * 2 - 1] = '\n';

            // Output hex data content.
            for (std::size_t idx = 0; idx < hex_lines - 2; ++idx)
            {
                const std::size_t line = hex_dump_line_length * (idx + 2);
                hex_dump.replace(line, 11, get_hex(idx * line_length, 8) + '|');
                for (std::size_t i = 0; i < line_length && size != 0; ++i)
                {
                    if (i < mean_length) {
                        hex_dump.replace(line + 12 + i * 3, 2, get_hex(static_cast<std::size_t>(*pSource), 2));
                    }
                    else { hex_dump.replace(line + 13 + i * 3, 2, get_hex(static_cast<std::size_t>(*pSource), 2)); }
                    pSource++;
                    size--;
                }

                const std::size_t hex_data = line_length * 3;
                pSource -= line_length;
                size += line_length;
                for (std::size_t i = 0; i < line_length && size != 0; ++i)
                { // bug with cyrillic symbols...
                    if (i < mean_length) {
                        if (isprint(static_cast<int32_t>(*pSource))) {
                            hex_dump.replace(line + hex_data + 17 + i, 1, pSource, 1);
                        } else {
                            hex_dump.replace(line + hex_data + 17 + i, 1, 1, '.');
                        }
                    } else {
                        if (isprint(static_cast<int32_t>(*pSource))) {
                            hex_dump.replace(line + hex_data + 18 + i, 1, pSource, 1);
                        } else {
                            hex_dump.replace(line + hex_data + 18 + i, 1, 1, '.');
                        }
                    };
                    pSource++;
                    size--;
                }
                hex_dump[line + hex_dump_line_length - 1] = '\n';
            }

            __common_log(message, '\n', hex_dump, '\n');
        }


        static void SetErrorStrings() noexcept
        {
            errors.insert( std::make_pair(EPERM,           "Operation not permitted."));
            errors.insert( std::make_pair(ENOENT,          "No such file or directory."));
            errors.insert( std::make_pair(ESRCH,           "No such process."));
            errors.insert( std::make_pair(EINTR,           "Interrupted system call."));
            errors.insert( std::make_pair(EIO,             "I/O error."));
            errors.insert( std::make_pair(ENXIO,           "No such device or address."));
            errors.insert( std::make_pair(E2BIG,           "Arg list too long."));
            errors.insert( std::make_pair(ENOEXEC,         "Exec format error."));
            errors.insert( std::make_pair(EBADF,           "Bad file number."));
            errors.insert( std::make_pair(ECHILD,          "No child processes."));
            errors.insert( std::make_pair(EAGAIN,          "Try again."));
            errors.insert( std::make_pair(ENOMEM,          "Out of memory."));
            errors.insert( std::make_pair(EACCES,          "Permission denied."));
            errors.insert( std::make_pair(EFAULT,          "Bad address."));
            errors.insert( std::make_pair(ENOTBLK,         "Block device required."));
            errors.insert( std::make_pair(EBUSY,           "Device or resource busy."));
            errors.insert( std::make_pair(EEXIST,          "File exists."));
            errors.insert( std::make_pair(EXDEV,           "Cross-device link."));
            errors.insert( std::make_pair(ENODEV,          "No such device."));
            errors.insert( std::make_pair(ENOTDIR,         "Not a directory."));
            errors.insert( std::make_pair(EISDIR,          "Is a directory."));
            errors.insert( std::make_pair(EINVAL,          "Invalid argument."));
            errors.insert( std::make_pair(ENFILE,          "File table overflow."));
            errors.insert( std::make_pair(EMFILE,          "Too many open files."));
            errors.insert( std::make_pair(ENOTTY,          "Not a typewriter."));
            errors.insert( std::make_pair(ETXTBSY,         "Text file busy."));
            errors.insert( std::make_pair(EFBIG,           "File too large."));
            errors.insert( std::make_pair(ENOSPC,          "No space left on device."));
            errors.insert( std::make_pair(ESPIPE,          "Illegal seek."));
            errors.insert( std::make_pair(EROFS,           "Read-only file system."));
            errors.insert( std::make_pair(EMLINK,          "Too many links."));
            errors.insert( std::make_pair(EPIPE,           "Broken pipe."));
            errors.insert( std::make_pair(EDEADLOCK,       "Resource deadlock would occur."));
            errors.insert( std::make_pair(ENAMETOOLONG,    "File name too long."));
            errors.insert( std::make_pair(ENOLCK,          "No record locks available."));
            errors.insert( std::make_pair(ENOSYS,          "Function not implemented."));
            errors.insert( std::make_pair(ENOTEMPTY,       "Directory not empty."));
            errors.insert( std::make_pair(ELOOP,           "Too many symbolic links encountered."));
            errors.insert( std::make_pair(EWOULDBLOCK,     "Operation would block."));
            errors.insert( std::make_pair(ENOMSG,          "No message of desired type."));
            errors.insert( std::make_pair(EIDRM,           "Identifier removed."));
            errors.insert( std::make_pair(ECHRNG,          "Channel number out of range."));
            errors.insert( std::make_pair(EL2NSYNC,        "Level 2 not synchronized."));
            errors.insert( std::make_pair(EL3HLT,          "Level 3 halted."));
            errors.insert( std::make_pair(EL3RST,          "Level 3 reset."));
            errors.insert( std::make_pair(ELNRNG,          "Link number out of range."));
            errors.insert( std::make_pair(EUNATCH,         "Protocol driver not attached."));
            errors.insert( std::make_pair(ENOCSI,          "No CSI structure available."));
            errors.insert( std::make_pair(EL2HLT,          "Level 2 halted."));
            errors.insert( std::make_pair(EBADE,           "Invalid exchange."));
            errors.insert( std::make_pair(EBADR,           "Invalid request descriptor."));
            errors.insert( std::make_pair(EXFULL,          "Exchange full."));
            errors.insert( std::make_pair(ENOANO,          "No anode."));
            errors.insert( std::make_pair(EBADRQC,         "Invalid request code."));
            errors.insert( std::make_pair(EBADSLT,         "Invalid slot."));
            errors.insert( std::make_pair(EBFONT,          "Bad font file format."));
            errors.insert( std::make_pair(ENOSTR,          "Device not a stream."));
            errors.insert( std::make_pair(ENODATA,         "No data available."));
            errors.insert( std::make_pair(ETIME,           "Timer expired."));
            errors.insert( std::make_pair(ENOSR,           "Out of streams resources."));
            errors.insert( std::make_pair(ENONET,          "Machine is not on the network."));
            errors.insert( std::make_pair(ENOPKG,          "Package not installed."));
            errors.insert( std::make_pair(EREMOTE,         "Object is remote."));
            errors.insert( std::make_pair(ENOLINK,         "Link has been severed."));
            errors.insert( std::make_pair(EADV,            "Advertise error."));
            errors.insert( std::make_pair(ESRMNT,          "Srmount error."));
            errors.insert( std::make_pair(ECOMM ,          "Communication error on send."));
            errors.insert( std::make_pair(EPROTO,          "Protocol error."));
            errors.insert( std::make_pair(EMULTIHOP,       "Multihop attempted."));
            errors.insert( std::make_pair(EDOTDOT,         "RFS specific error."));
            errors.insert( std::make_pair(EBADMSG,         "Not a data message."));
            errors.insert( std::make_pair(EOVERFLOW,       "Value too large for defined data type."));
            errors.insert( std::make_pair(ENOTUNIQ,        "Name not unique on network."));
            errors.insert( std::make_pair(EBADFD,          "File descriptor in bad state."));
            errors.insert( std::make_pair(EREMCHG,         "Remote address changed."));
            errors.insert( std::make_pair(ELIBACC,         "Can not access a needed shared library."));
            errors.insert( std::make_pair(ELIBBAD,         "Accessing a corrupted shared library."));
            errors.insert( std::make_pair(ELIBSCN,         ".lib section in a.out corrupted."));
            errors.insert( std::make_pair(ELIBMAX,         "Attempting to link in too many shared libraries."));
            errors.insert( std::make_pair(ELIBEXEC,        "Cannot exec a shared library directly."));
            errors.insert( std::make_pair(ERESTART,        "Interrupted system call should be restarted."));
            errors.insert( std::make_pair(ESTRPIPE,        "Streams pipe error."));
            errors.insert( std::make_pair(EUSERS,          "Too many users."));
            errors.insert( std::make_pair(ENOTSOCK,        "Socket operation on non-socket."));
            errors.insert( std::make_pair(EDESTADDRREQ,    "Destination address required."));
            errors.insert( std::make_pair(EMSGSIZE,        "Message too long."));
            errors.insert( std::make_pair(EPROTOTYPE,      "Protocol wrong type for socket."));
            errors.insert( std::make_pair(ENOPROTOOPT,     "Protocol not available."));
            errors.insert( std::make_pair(EPROTONOSUPPORT, "Protocol not supported."));
            errors.insert( std::make_pair(ESOCKTNOSUPPORT, "Socket type not supported."));
            errors.insert( std::make_pair(EOPNOTSUPP,      "Operation not supported on transport endpoint."));
            errors.insert( std::make_pair(EPFNOSUPPORT,    "Protocol family not supported."));
            errors.insert( std::make_pair(EAFNOSUPPORT,    "Address family not supported by protocol."));
            errors.insert( std::make_pair(EADDRINUSE,      "Address already in use."));
            errors.insert( std::make_pair(EADDRNOTAVAIL,   "Cannot assign requested address."));
            errors.insert( std::make_pair(ENETDOWN,        "Network is down."));
            errors.insert( std::make_pair(ENETUNREACH,     "Network is unreachable."));
            errors.insert( std::make_pair(ENETRESET,       "Network dropped connection because of reset."));
            errors.insert( std::make_pair(ECONNABORTED,    "Software caused connection abort."));
            errors.insert( std::make_pair(ECONNRESET,      "Connection reset by peer."));
            errors.insert( std::make_pair(ENOBUFS,         "No buffer space available."));
            errors.insert( std::make_pair(EISCONN,         "Transport endpoint is already connected."));
            errors.insert( std::make_pair(ENOTCONN,        "Transport endpoint is not connected."));
            errors.insert( std::make_pair(ESHUTDOWN,       "Cannot send after transport endpoint shutdown."));
            errors.insert( std::make_pair(ETOOMANYREFS,    "Too many references: cannot splice."));
            errors.insert( std::make_pair(ETIMEDOUT,       "Connection timed out."));
            errors.insert( std::make_pair(ECONNREFUSED,    "Connection refused."));
            errors.insert( std::make_pair(EHOSTDOWN,       "Host is down."));
            errors.insert( std::make_pair(EHOSTUNREACH,    "No route to host."));
            errors.insert( std::make_pair(EALREADY,        "Operation already in progress."));
            errors.insert( std::make_pair(EINPROGRESS,     "Operation now in progress."));
            errors.insert( std::make_pair(ESTALE,          "Stale NFS file handle."));
            errors.insert( std::make_pair(EUCLEAN,         "Structure needs cleaning."));
            errors.insert( std::make_pair(ENOTNAM,         "Not a XENIX named type file."));
            errors.insert( std::make_pair(ENAVAIL,         "No XENIX semaphores available."));
            errors.insert( std::make_pair(EISNAM,          "Is a named type file."));
            errors.insert( std::make_pair(EREMOTEIO,       "Remote I/O error."));
            errors.insert( std::make_pair(EDQUOT,          "Quota exceeded."));
            errors.insert( std::make_pair(ENOMEDIUM,       "No medium found."));
            errors.insert( std::make_pair(EMEDIUMTYPE,     "Wrong medium type."));
            errors.insert( std::make_pair(ECANCELED,       "Operation canceled."));
            errors.insert( std::make_pair(ENOKEY,          "Required key not available."));
            errors.insert( std::make_pair(EKEYEXPIRED,     "Key has expired."));
            errors.insert( std::make_pair(EKEYREVOKED,     "Key has been revoked."));
            errors.insert( std::make_pair(EKEYREJECTED,    "Key was rejected by service."));
            errors.insert( std::make_pair(EOWNERDEAD,      "Owner died."));
            errors.insert( std::make_pair(ENOTRECOVERABLE, "State not recoverable."));
            errors.insert( std::make_pair(ERFKILL,         "Operation not possible due to RF-kill."));
            errors.insert( std::make_pair(EHWPOISON,       "Memory page has hardware error."));
        }

    }  // namespace log.
}  // namespace analyzer.
